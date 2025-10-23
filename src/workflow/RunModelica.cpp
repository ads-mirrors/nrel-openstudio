/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"
#include "utilities/core/ApplicationPathHelpers.hpp"
#include "../utilities/core/Filesystem.hpp"
#include <utilities/idd/OS_ThermalZone_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>
#include <fmt/format.h>
#include <cstdlib>
#include <set>
#include <stdexcept>

namespace openstudio {

namespace {
  struct ModelicaSetup
  {
    std::vector<openstudio::path> files;
    std::vector<openstudio::path> searchPaths;
  };

  std::string modelicaStringLiteral(const openstudio::path& path) {
    const std::string raw = path.generic_string();
    std::string escaped;
    escaped.reserve(raw.size() * 2);
    for (const char ch : raw) {
      if (ch == '\\' || ch == '"') {
        escaped.push_back('\\');
      }
      escaped.push_back(ch);
    }
    return fmt::format("\"{}\"", escaped);
  }

  void dedupePaths(std::vector<openstudio::path>& paths) {
    std::set<std::string> seen;
    std::vector<openstudio::path> unique;
    unique.reserve(paths.size());

    for (const auto& path : paths) {
      if (path.empty()) {
        continue;
      }
      const auto absolutePath = boost::filesystem::absolute(path);
      const std::string key = absolutePath.generic_string();
      if (seen.insert(key).second) {
        unique.push_back(absolutePath);
      }
    }

    paths = std::move(unique);
  }

  ModelicaSetup getModelicaSetup(const WorkflowJSON& workflowJSON, const boost::optional<openstudio::path>& finalSeedModelicaFile) {
    ModelicaSetup setup;

    auto addSearchPath = [&setup](const openstudio::path& dir) {
      if (dir.empty()) {
        return;
      }
      setup.searchPaths.push_back(dir);
    };

    auto addFileToLoad = [&setup, &addSearchPath](const openstudio::path& file) {
      if (file.empty()) {
        return;
      }
      const auto absoluteFile = boost::filesystem::absolute(file);
      setup.files.push_back(absoluteFile);
      addSearchPath(absoluteFile.parent_path());
    };

    const auto handleDirectory = [&addFileToLoad, &addSearchPath](const openstudio::path& dir) {
      const auto absoluteDir = boost::filesystem::absolute(dir);
      addSearchPath(absoluteDir);
      const auto packageMo = absoluteDir / toPath("package.mo");
      if (openstudio::filesystem::exists(packageMo)) {
        addFileToLoad(packageMo);
      }
    };

    const auto mblPath = getMBLPath();
    if (!mblPath.empty() && openstudio::filesystem::exists(mblPath)) {
      if (openstudio::filesystem::is_directory(mblPath)) {
        handleDirectory(mblPath);
      } else {
        addFileToLoad(mblPath);
      }
    }

    for (const auto& packageSpec : workflowJSON.modelicaPackages()) {
      openstudio::path resolved;
      if (packageSpec.is_absolute()) {
        resolved = packageSpec;
      } else {
        if (auto located = workflowJSON.findFile(packageSpec)) {
          resolved = *located;
        }
      }

      if (resolved.empty()) {
        throw std::runtime_error(fmt::format("Modelica package '{}' could not be resolved", toString(packageSpec)));
      }

      if (!openstudio::filesystem::exists(resolved)) {
        throw std::runtime_error(fmt::format("Modelica package '{}' does not exist at '{}'", toString(packageSpec), resolved.generic_string()));
      }

      if (openstudio::filesystem::is_directory(resolved)) {
        handleDirectory(resolved);
      } else {
        addFileToLoad(resolved);
      }
    }

    if (finalSeedModelicaFile) {
      if (openstudio::filesystem::is_directory(*finalSeedModelicaFile)) {
        handleDirectory(*finalSeedModelicaFile);
      } else {
        addFileToLoad(*finalSeedModelicaFile);
      }
    } else if (auto seedModelicaFile = workflowJSON.seedModelicaFile()) {
      if (auto resolved = workflowJSON.findFile(seedModelicaFile.get())) {
        if (openstudio::filesystem::is_directory(*resolved)) {
          handleDirectory(*resolved);
        } else {
          addFileToLoad(*resolved);
        }
      }
    }

    dedupePaths(setup.files);
    dedupePaths(setup.searchPaths);

    return setup;
  }

  std::string buildSetModelicaPathCommand(const std::vector<openstudio::path>& searchPaths) {
    std::string expression = "getModelicaPath()";

    std::vector<std::string> additions;
    additions.emplace_back("getHomeDirectoryPath() + \"/.openmodelica/libraries/\"");
    for (const auto& searchPath : searchPaths) {
      additions.emplace_back(modelicaStringLiteral(searchPath));
    }

#ifdef _WIN32
    constexpr const char* separatorLiteral = "\";\"";
#else
    constexpr const char* separatorLiteral = "\":\"";
#endif

    for (const auto& addition : additions) {
      expression += std::string(" + ") + separatorLiteral + " + " + addition;
    }

    return fmt::format("setModelicaPath({});\n", expression);
  }

  openstudio::path createModelicaScript(const WorkflowJSON& workflowJSON, const ModelicaSetup& setup, const measure::ModelicaParameters& params) {
    auto seedModelicaModel = workflowJSON.seedModelicaModel();
    // There is a check for seedModelicaModel, prior to reaching this point.
    // In other words, we shouldn't be trying to "runModelica" if there is no seed model.
    OS_ASSERT(seedModelicaModel);

    constexpr auto mosPath = "run.mos";
    std::ofstream mosFile(mosPath);

    mosFile << buildSetModelicaPathCommand(setup.searchPaths);
    for (const auto& file : setup.files) {
      mosFile << fmt::format("loadFile({});\n", modelicaStringLiteral(file));
    }
    const auto allParams = params.getAllParameters();
    for (const auto& param : allParams) {
      mosFile << fmt::format("setParameterValue({}, {}, {});\n", param.model(), param.key(), param.value());
    }
    mosFile << fmt::format("simulate({}, stopTime=604800, stepSize=10);", *seedModelicaModel);
    mosFile.close();

    return mosPath;
  }
}  // namespace

void OSWorkflow::runModelica() {
  const auto curDirPath = boost::filesystem::current_path();
  try {
    auto runDirPath = workflowJSON.absoluteRunDir();
    boost::filesystem::current_path(runDirPath);
    //PrepareRunDirResults runDirResults(runDirPath);
    LOG(Info, "Starting Modelica simulation in run directory: " << runDirPath);

    state = State::Modelica;

    int result = 0;
    OS_ASSERT(workspace_);
    //const auto params = getModelicaParams(workflowJSON, *workspace_);
    const auto params = runner.modelicaParameters();
    const auto setup = getModelicaSetup(workflowJSON, m_latestModelicaFilePath);
    const auto script_path = createModelicaScript(workflowJSON, setup, params);
    const auto cmd = fmt::format("{} {}", getOMCExecutable().string(), script_path.string());

    detailedTimeBlock("Running Modelica", [&cmd, &result] { result = std::system(cmd.c_str()); });  // NOLINT

    LOG(Info, "Modelica returned '" << result << "'");
    if (result != 0) {
      LOG(Warn, "Modelica returned a non-zero exit code (" << result << "). Check the Modelica log");
    }

  } catch (const std::exception& e) {
    boost::filesystem::current_path(curDirPath);
    LOG_AND_THROW(e.what());
  }
}

}  // namespace openstudio
