/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"
#include "utilities/core/ApplicationPathHelpers.hpp"
#include "utilities/idd/Zone_FieldEnums.hxx"
#include <algorithm>
#include <numeric>
#include <utilities/idd/OS_ThermalZone_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>
#include <fmt/format.h>
#include <cstdlib>

namespace openstudio {

namespace {
  struct ModelicaParam
  {
    std::string model;
    std::string key;
    std::string value;
  };

  using ModelicaParams = std::vector<ModelicaParam>;
  using ModelicaFiles = std::vector<openstudio::path>;

  std::vector<WorkspaceObject> getConditionedZones(const Workspace& workspace) {
    // We want a list of zones to condition.
    // If a zone is not included in the floor area (plenum), then it is not conditioned
    const auto isUnconditioned = [](const WorkspaceObject& zone) {
      const auto floorArea = zone.getString(ZoneFields::PartofTotalFloorArea);
      if (floorArea) {
        std::string lowerFloorArea = *floorArea;
        boost::to_lower(lowerFloorArea);
        if (lowerFloorArea == "no") {
          return true;
        }
      }
      return false;
    };

    auto zones = workspace.getObjectsByType(IddObjectType::Zone);
    const auto e = std::ranges::remove_if(zones, isUnconditioned);
    zones.erase(e.begin(), e.end());

    return zones;
  }

  std::string getZoneNamesString(const std::vector<WorkspaceObject>& zones) {
    std::stringstream ss;

    ss << "{";
    if (!zones.empty()) {
      // Create a comma separated list of the zone names to condition.
      // Names will be quoted.
      ss << std::accumulate(std::next(zones.begin()), zones.end(), fmt::format("\"{}\"", zones.front().nameString()),
                            [](const std::string& a, const WorkspaceObject& obj) { return fmt::format("{}, \"{}\"", a, obj.nameString()); });
    }
    ss << "}";

    return ss.str();
  }

  ModelicaParams getModelicaParams(const WorkflowJSON& workflowJSON, [[maybe_unused]] const Workspace& workspace) {
    const auto seedModelicaModel = workflowJSON.seedModelicaModel();
    OS_ASSERT(seedModelicaModel);

    const auto idfPath = workflowJSON.absoluteRunDir() / "in.idf";
    const auto conditionedZones = getConditionedZones(workspace);

    std::vector<ModelicaParam> params;

    params.push_back({seedModelicaModel.get(), "idfPath", fmt::format("\"{}\"", idfPath.string())});
    params.push_back({seedModelicaModel.get(), "zoneCount", toString(conditionedZones.size())});
    params.push_back({seedModelicaModel.get(), "zoneNames", getZoneNamesString(conditionedZones)});

    return params;
  }

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

  openstudio::path createModelicaScript(const WorkflowJSON& workflowJSON, const ModelicaFiles& files, const ModelicaParams& params) {
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
    for (const auto& param : params) {
      mosFile << fmt::format("setParameterValue({}, {}, {});\n", param.model, param.key, param.value);
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
    const auto params = getModelicaParams(workflowJSON, *workspace_);
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
