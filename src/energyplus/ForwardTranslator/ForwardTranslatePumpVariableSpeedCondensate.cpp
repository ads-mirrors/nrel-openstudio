/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "../ForwardTranslator.hpp"

#include "../../model/PumpVariableSpeedCondensate.hpp"
#include "../../model/PumpVariableSpeedCondensate_Impl.hpp"
#include "../../model/Model.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/Node.hpp"
#include "../../model/Node_Impl.hpp"
#include "../../model/Curve.hpp"
#include "../../model/Curve_Impl.hpp"

#include "../../utilities/core/Assert.hpp"

#include <utilities/idd/Pump_VariableSpeed_Condensate_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

using namespace std;

namespace openstudio {

namespace energyplus {

  boost::optional<IdfObject> ForwardTranslator::translatePumpVariableSpeedCondensate(PumpVariableSpeedCondensate& modelObject) {
    boost::optional<std::string> s;
    boost::optional<double> value;
    OptionalSchedule schedule;

    IdfObject idfObject(IddObjectType::Pump_VariableSpeed_Condensate);

    m_idfObjects.push_back(idfObject);

    // Name

    s = modelObject.name();
    if (s) {
      idfObject.setName(*s);
    }

    // InletNodeName

    if (boost::optional<ModelObject> mo = modelObject.inletModelObject()) {
      if (boost::optional<Node> node = mo->optionalCast<Node>()) {
        idfObject.setString(Pump_VariableSpeed_CondensateFields::InletNodeName, node->name().get());
      }
    }

    // OutletNodeName

    if (boost::optional<ModelObject> mo = modelObject.outletModelObject()) {
      if (boost::optional<Node> node = mo->optionalCast<Node>()) {
        idfObject.setString(Pump_VariableSpeed_CondensateFields::OutletNodeName, node->name().get());
      }
    }

    // RatedFlowRate

    if (modelObject.isRatedFlowRateAutosized()) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::DesignMaximumFlowRate, "Autosize");
    } else if ((value = modelObject.ratedFlowRate())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignMaximumFlowRate, value.get());
    }

    // RatedPumpHead

    if ((value = modelObject.ratedPumpHead())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignPumpHead, value.get());
    }

    // RatedPowerConsumption

    if (modelObject.isRatedPowerConsumptionAutosized()) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::DesignPowerConsumption, "Autosize");
    } else if ((value = modelObject.ratedPowerConsumption())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignPowerConsumption, value.get());
    }

    // MotorEfficiency

    if ((value = modelObject.motorEfficiency())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::MotorEfficiency, value.get());
    }

    // FractionofMotorInefficienciestoFluidStream

    if ((value = modelObject.fractionofMotorInefficienciestoFluidStream())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::FractionofMotorInefficienciestoFluidStream, value.get());
    }

    // Coefficient1ofthePartLoadPerformanceCurve

    if ((value = modelObject.coefficient1ofthePartLoadPerformanceCurve())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::Coefficient1ofthePartLoadPerformanceCurve, value.get());
    }

    // Coefficient2ofthePartLoadPerformanceCurve

    if ((value = modelObject.coefficient2ofthePartLoadPerformanceCurve())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::Coefficient2ofthePartLoadPerformanceCurve, value.get());
    }

    // Coefficient3ofthePartLoadPerformanceCurve

    if ((value = modelObject.coefficient3ofthePartLoadPerformanceCurve())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::Coefficient3ofthePartLoadPerformanceCurve, value.get());
    }

    // Coefficient4ofthePartLoadPerformanceCurve

    if ((value = modelObject.coefficient4ofthePartLoadPerformanceCurve())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::Coefficient4ofthePartLoadPerformanceCurve, value.get());
    }

    // MinimumFlowRate

    if ((value = modelObject.minimumFlowRate())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignMinimumFlowRate, value.get());
    }

    // PumpControlType

    if ((s = modelObject.pumpControlType())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::PumpControlType, s.get());
    }

    // PumpFlowRateSchedule

    if ((schedule = modelObject.pumpFlowRateSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName, schedule->name().get());
    }

    // PumpCurve

    if (OptionalCurve curve = modelObject.pumpCurve()) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::PumpCurveName, curve->name().get());
    }

    // ImpellerDiameter

    if ((value = modelObject.impellerDiameter())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::ImpellerDiameter, value.get());
    }

    // VFDControlType

    if ((s = modelObject.vFDControlType())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::VFDControlType, s.get());
    }

    // PumpRPMSchedule

    if ((schedule = modelObject.pumpRPMSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::PumpRPMScheduleName, schedule->name().get());
    }

    // MinimumPressureSchedule

    if ((schedule = modelObject.minimumPressureSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::MinimumPressureSchedule, schedule->name().get());
    }

    // MaximumPressureSchedule

    if ((schedule = modelObject.maximumPressureSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::MaximumPressureSchedule, schedule->name().get());
    }

    // MinimumRPMSchedule

    if ((schedule = modelObject.minimumRPMSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::MinimumRPMSchedule, schedule->name().get());
    }

    // MaximumRPMSchedule

    if ((schedule = modelObject.maximumRPMSchedule())) {
      idfObject.setString(Pump_VariableSpeed_CondensateFields::MaximumRPMSchedule, schedule->name().get());
    }

    if ((value = modelObject.skinLossRadiativeFraction())) {
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::SkinLossRadiativeFraction, value.get());
    }

    {
      s = modelObject.designPowerSizingMethod();
      idfObject.setString(Pump_VariableSpeed_CondensateFields::DesignPowerSizingMethod, s.get());
    }

    {
      value = modelObject.designElectricPowerPerUnitFlowRate();
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignElectricPowerperUnitFlowRate, value.get());
    }

    {
      value = modelObject.designShaftPowerPerUnitFlowRatePerUnitHead();
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignShaftPowerperUnitFlowRateperUnitHead, value.get());
    }

    {
      value = modelObject.designMinimumFlowRateFraction();
      idfObject.setDouble(Pump_VariableSpeed_CondensateFields::DesignMinimumFlowRateFraction, value.get());
    }

    // End Use Subcategory
    idfObject.setString(Pump_VariableSpeed_CondensateFields::EndUseSubcategory, modelObject.endUseSubcategory());

    return idfObject;
  }

}  // namespace energyplus
}  // namespace openstudio
