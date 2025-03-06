/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OSWorkflow.hpp"

namespace openstudio {

void OSWorkflow::runModelicaMeasures() {
  state = State::ModelicaMeasures;

  LOG(Info, "Beginning to execute Modelica Measures");
  applyMeasures(MeasureType::ModelicaMeasure, false);
  LOG(Info, "Finished applying Modelica Measures.");

  communicateMeasureAttributes();
}

}  // namespace openstudio
