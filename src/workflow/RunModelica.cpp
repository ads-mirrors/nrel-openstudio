/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"
#include "utilities/core/ApplicationPathHelpers.hpp"
#include <utilities/idd/OS_ThermalZone_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>
#include <fmt/format.h>
#include <cstdlib>

namespace openstudio {

namespace {
  using ModelicaFiles = std::vector<openstudio::path>;

  ModelicaFiles getModelicaFiles(const WorkflowJSON& workflowJSON) {
    ModelicaFiles files;

    const auto mblPath = getMBLPath();
    if (openstudio::filesystem::exists(mblPath)) {
      files.push_back(getMBLPath());
    }

    auto seedModelicaFile = workflowJSON.seedModelicaFile();
    if (seedModelicaFile) {
      seedModelicaFile = workflowJSON.findFile(seedModelicaFile.get());
      if (seedModelicaFile) {
        files.push_back(*seedModelicaFile);
      }
    }

    return files;
  }

  openstudio::path createModelicaScript(const WorkflowJSON& workflowJSON, const ModelicaFiles& files, const measure::ModelicaParameters& params) {
    auto seedModelicaModel = workflowJSON.seedModelicaModel();
    // There is a check for seedModelicaModel, prior to reaching this point.
    // In other words, we shouldn't be trying to "runModelica" if there is no seed model.
    OS_ASSERT(seedModelicaModel);

    constexpr auto mosPath = "run.mos";
    std::ofstream mosFile(mosPath);

    mosFile << "setModelicaPath(getHomeDirectoryPath() + \"/.openmodelica/libraries/\");\n";
    for (const auto& file : files) {
      mosFile << fmt::format("loadFile(\"{}\");\n", file.string());
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
    const auto files = getModelicaFiles(workflowJSON);
    const auto script_path = createModelicaScript(workflowJSON, files, params);
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
