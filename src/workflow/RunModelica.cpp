/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"
#include <fmt/format.h>
#include <cstdlib>

namespace openstudio {

namespace {
  openstudio::path createModelicaScript([[maybe_unused]] const WorkflowJSON& workflowJSON) {
    constexpr auto content = R"(
setModelicaPath(getHomeDirectoryPath() + "/.openmodelica/libraries/");
loadFile("{mblPath}");
loadFile("{seedModelicaFile}");
setParameterValue(Template_IdealHeat.Model, zoneNames, {{"LIVING ZONE"}});
simulate(Template_IdealHeat.Model, stopTime=604800, stepSize=0.1);
  )";

    const auto seedModelicaFile = workflowJSON.seedModelicaFile();
    OS_ASSERT(seedModelicaFile);
    const auto mblPath = std::getenv("MBL_PATH");
    const auto formatted_content = fmt::format(content, fmt::arg("mblPath", mblPath), fmt::arg("seedModelicaFile", seedModelicaFile->string()));

    constexpr auto mosPath = "run.mos";
    std::ofstream mosFile(mosPath);
    mosFile << formatted_content;
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
    const auto script = createModelicaScript(workflowJSON);
    const auto cmd = fmt::format("{} {}", getOMCExecutable().string(), script.string());

    detailedTimeBlock("Running Modelica", [&cmd, &result] { result = std::system(cmd.c_str()); });

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
