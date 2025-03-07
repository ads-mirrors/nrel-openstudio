/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"

namespace openstudio {

namespace {
  void setDefaultParameters(const measure::OSRunner& runner) {
    const auto workflow = runner.workflow();
    const auto seedModelicaModel = workflow.seedModelicaModel();
    OS_ASSERT(seedModelicaModel);

    const auto idfPath = workflow.absoluteRunDir() / "in.idf";
    auto params = runner.modelicaParameters();
    params.setParameterValue(*seedModelicaModel, "idfPath", fmt::format("\"{}\"", idfPath.string()));
  }
}  // namespace

void OSWorkflow::runModelicaMeasures() {
  state = State::ModelicaMeasures;

  LOG(Info, "Beginning to execute Modelica Measures");
  setDefaultParameters(runner);
  applyMeasures(MeasureType::ModelicaMeasure, false);
  LOG(Info, "Finished applying Modelica Measures.");

  communicateMeasureAttributes();
}

}  // namespace openstudio
