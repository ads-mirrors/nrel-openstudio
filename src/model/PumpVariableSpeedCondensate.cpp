/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "PumpVariableSpeedCondensate.hpp"
#include "PumpVariableSpeedCondensate_Impl.hpp"

#include "Schedule.hpp"
#include "Schedule_Impl.hpp"
#include "Node.hpp"
#include "Node_Impl.hpp"
#include "Curve.hpp"
#include "Curve_Impl.hpp"
#include "CurveLinear.hpp"
#include "CurveLinear_Impl.hpp"
#include "CurveQuadratic.hpp"
#include "CurveQuadratic_Impl.hpp"
#include "CurveCubic.hpp"
#include "CurveCubic_Impl.hpp"
#include "CurveQuartic.hpp"
#include "CurveQuartic_Impl.hpp"
#include "ThermalZone.hpp"
#include "ThermalZone_Impl.hpp"

#include <utilities/idd/IddFactory.hxx>

#include <utilities/idd/OS_Pump_VariableSpeed_Condensate_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>
#include "../utilities/core/Assert.hpp"
#include "../utilities/data/DataEnums.hpp"

namespace openstudio {
namespace model {

  namespace detail {

    PumpVariableSpeedCondensate_Impl::PumpVariableSpeedCondensate_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(idfObject, model, keepHandle) {
      OS_ASSERT(idfObject.iddObject().type() == PumpVariableSpeedCondensate::iddObjectType());
    }

    PumpVariableSpeedCondensate_Impl::PumpVariableSpeedCondensate_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model,
                                                                       bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {
      OS_ASSERT(other.iddObject().type() == PumpVariableSpeedCondensate::iddObjectType());
    }

    PumpVariableSpeedCondensate_Impl::PumpVariableSpeedCondensate_Impl(const PumpVariableSpeedCondensate_Impl& other, Model_Impl* model,
                                                                       bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {}

    const std::vector<std::string>& PumpVariableSpeedCondensate_Impl::outputVariableNames() const {
      static const std::vector<std::string> result{
        "Pump Electricity Rate", "Pump Electricity Energy", "Pump Shaft Power", "Pump Fluid Heat Gain Rate", "Pump Fluid Heat Gain Energy",
        "Pump Outlet Temperature", "Pump Mass Flow Rate", "Pump Operating Pumps Count",

        // The Key is the Pump, not the zone, so it's right to report here
        // EnergyPlus/Pumps.cc::GetPumpInput()
        // TODO: Implement this check and make not static above once ModelObject return type has changed
        //if (! p.zone().empty() ) {
        "Pump Zone Total Heating Rate", "Pump Zone Total Heating Energy", "Pump Zone Convective Heating Rate", "Pump Zone Radiative Heating Rate"
        // }
      };
      return result;
    }

    std::vector<ScheduleTypeKey> PumpVariableSpeedCondensate_Impl::getScheduleTypeKeys(const Schedule& schedule) const {
      std::vector<ScheduleTypeKey> result;
      UnsignedVector fieldIndices = getSourceIndices(schedule.handle());
      UnsignedVector::const_iterator b(fieldIndices.begin());
      UnsignedVector::const_iterator e(fieldIndices.end());
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Pump Flow Rate"));
      }
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::PumpRPMScheduleName) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Pump RPM"));
      }
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::MinimumPressureSchedule) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Minimum Pressure"));
      }
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::MaximumPressureSchedule) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Maximum Pressure"));
      }
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::MinimumRPMSchedule) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Minimum RPM"));
      }
      if (std::find(b, e, OS_Pump_VariableSpeed_CondensateFields::MaximumRPMSchedule) != e) {
        result.push_back(ScheduleTypeKey("PumpVariableSpeedCondensate", "Maximum RPM"));
      }
      return result;
    }

    IddObjectType PumpVariableSpeedCondensate_Impl::iddObjectType() const {
      return PumpVariableSpeedCondensate::iddObjectType();
    }

    std::vector<ModelObject> PumpVariableSpeedCondensate_Impl::children() const {
      ModelObjectVector result;
      if (OptionalCurve curve = pumpCurve()) {
        result.push_back(*curve);
      }
      return result;
    }

    unsigned PumpVariableSpeedCondensate_Impl::inletPort() const {
      return OS_Pump_VariableSpeed_CondensateFields::InletNodeName;
    }

    unsigned PumpVariableSpeedCondensate_Impl::outletPort() const {
      return OS_Pump_VariableSpeed_CondensateFields::OutletNodeName;
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::ratedFlowRate() const {
      return getDouble(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate, true);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedFlowRateDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedFlowRateAutosized() const {
      bool result = false;
      boost::optional<std::string> value = getString(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate, true);
      if (value) {
        result = openstudio::istringEqual(value.get(), "autosize");
      }
      return result;
    }

    double PumpVariableSpeedCondensate_Impl::ratedPumpHead() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::RatedPumpHead, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedPumpHeadDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::RatedPumpHead);
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::ratedPowerConsumption() const {
      return getDouble(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption, true);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedPowerConsumptionDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedPowerConsumptionAutosized() const {
      bool result = false;
      boost::optional<std::string> value = getString(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption, true);
      if (value) {
        result = openstudio::istringEqual(value.get(), "autosize");
      }
      return result;
    }

    double PumpVariableSpeedCondensate_Impl::motorEfficiency() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::MotorEfficiency, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isMotorEfficiencyDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::MotorEfficiency);
    }

    double PumpVariableSpeedCondensate_Impl::fractionofMotorInefficienciestoFluidStream() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::FractionofMotorInefficienciestoFluidStream, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isFractionofMotorInefficienciestoFluidStreamDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::FractionofMotorInefficienciestoFluidStream);
    }

    double PumpVariableSpeedCondensate_Impl::coefficient1ofthePartLoadPerformanceCurve() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient1ofthePartLoadPerformanceCurve, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isCoefficient1ofthePartLoadPerformanceCurveDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::Coefficient1ofthePartLoadPerformanceCurve);
    }

    double PumpVariableSpeedCondensate_Impl::coefficient2ofthePartLoadPerformanceCurve() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient2ofthePartLoadPerformanceCurve, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isCoefficient2ofthePartLoadPerformanceCurveDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::Coefficient2ofthePartLoadPerformanceCurve);
    }

    double PumpVariableSpeedCondensate_Impl::coefficient3ofthePartLoadPerformanceCurve() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient3ofthePartLoadPerformanceCurve, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isCoefficient3ofthePartLoadPerformanceCurveDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::Coefficient3ofthePartLoadPerformanceCurve);
    }

    double PumpVariableSpeedCondensate_Impl::coefficient4ofthePartLoadPerformanceCurve() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient4ofthePartLoadPerformanceCurve, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isCoefficient4ofthePartLoadPerformanceCurveDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::Coefficient4ofthePartLoadPerformanceCurve);
    }

    double PumpVariableSpeedCondensate_Impl::minimumFlowRate() const {
      boost::optional<double> value = getDouble(OS_Pump_VariableSpeed_CondensateFields::MinimumFlowRate, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isMinimumFlowRateDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::MinimumFlowRate);
    }

    std::string PumpVariableSpeedCondensate_Impl::pumpControlType() const {
      boost::optional<std::string> value = getString(OS_Pump_VariableSpeed_CondensateFields::PumpControlType, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::isPumpControlTypeDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::PumpControlType);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::pumpFlowRateSchedule() const {
      return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName);
    }

    boost::optional<Curve> PumpVariableSpeedCondensate_Impl::pumpCurve() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Curve>(OS_Pump_VariableSpeed_CondensateFields::PumpCurveName);
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::impellerDiameter() const {
      return getDouble(OS_Pump_VariableSpeed_CondensateFields::ImpellerDiameter, true);
    }

    boost::optional<std::string> PumpVariableSpeedCondensate_Impl::vFDControlType() const {
      return getString(OS_Pump_VariableSpeed_CondensateFields::VFDControlType, true);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::pumpRPMSchedule() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::PumpRPMScheduleName);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::minimumPressureSchedule() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::MinimumPressureSchedule);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::maximumPressureSchedule() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::MaximumPressureSchedule);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::minimumRPMSchedule() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::MinimumRPMSchedule);
    }

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::maximumRPMSchedule() const {
      return getObject<PumpVariableSpeed>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::MaximumRPMSchedule);
    }

    bool PumpVariableSpeedCondensate_Impl::setRatedFlowRate(boost::optional<double> ratedFlowRate) {
      bool result(false);
      if (ratedFlowRate) {
        result = setDouble(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate, ratedFlowRate.get());
      } else {
        resetRatedFlowRate();
        result = true;
      }
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetRatedFlowRate() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate, "");
      OS_ASSERT(result);
    }

    void PumpVariableSpeedCondensate_Impl::autosizeRatedFlowRate() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedFlowRate, "autosize");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setRatedPumpHead(double ratedPumpHead) {
      bool result = setDouble(OS_Pump_VariableSpeed_CondensateFields::RatedPumpHead, ratedPumpHead);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetRatedPumpHead() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedPumpHead, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setRatedPowerConsumption(boost::optional<double> ratedPowerConsumption) {
      bool result(false);
      if (ratedPowerConsumption) {
        result = setDouble(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption, ratedPowerConsumption.get());
      } else {
        resetRatedPowerConsumption();
        result = true;
      }
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetRatedPowerConsumption() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption, "");
      OS_ASSERT(result);
    }

    void PumpVariableSpeedCondensate_Impl::autosizeRatedPowerConsumption() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedPowerConsumption, "autosize");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMotorEfficiency(double motorEfficiency) {
      bool result = setDouble(OS_Pump_VariableSpeed_CondensateFields::MotorEfficiency, motorEfficiency);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMotorEfficiency() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MotorEfficiency, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setFractionofMotorInefficienciestoFluidStream(double fractionofMotorInefficienciestoFluidStream) {
      bool result =
        setDouble(OS_Pump_VariableSpeed_CondensateFields::FractionofMotorInefficienciestoFluidStream, fractionofMotorInefficienciestoFluidStream);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetFractionofMotorInefficienciestoFluidStream() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::FractionofMotorInefficienciestoFluidStream, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setCoefficient1ofthePartLoadPerformanceCurve(double coefficient1ofthePartLoadPerformanceCurve) {
      bool result =
        setDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient1ofthePartLoadPerformanceCurve, coefficient1ofthePartLoadPerformanceCurve);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetCoefficient1ofthePartLoadPerformanceCurve() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::Coefficient1ofthePartLoadPerformanceCurve, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setCoefficient2ofthePartLoadPerformanceCurve(double coefficient2ofthePartLoadPerformanceCurve) {
      bool result =
        setDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient2ofthePartLoadPerformanceCurve, coefficient2ofthePartLoadPerformanceCurve);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetCoefficient2ofthePartLoadPerformanceCurve() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::Coefficient2ofthePartLoadPerformanceCurve, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setCoefficient3ofthePartLoadPerformanceCurve(double coefficient3ofthePartLoadPerformanceCurve) {
      bool result =
        setDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient3ofthePartLoadPerformanceCurve, coefficient3ofthePartLoadPerformanceCurve);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetCoefficient3ofthePartLoadPerformanceCurve() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::Coefficient3ofthePartLoadPerformanceCurve, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setCoefficient4ofthePartLoadPerformanceCurve(double coefficient4ofthePartLoadPerformanceCurve) {
      bool result =
        setDouble(OS_Pump_VariableSpeed_CondensateFields::Coefficient4ofthePartLoadPerformanceCurve, coefficient4ofthePartLoadPerformanceCurve);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetCoefficient4ofthePartLoadPerformanceCurve() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::Coefficient4ofthePartLoadPerformanceCurve, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMinimumFlowRate(double minimumFlowRate) {
      bool result = setDouble(OS_Pump_VariableSpeed_CondensateFields::MinimumFlowRate, minimumFlowRate);
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMinimumFlowRate() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MinimumFlowRate, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpControlType(const std::string& pumpControlType) {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::PumpControlType, pumpControlType);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetPumpControlType() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::PumpControlType, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpFlowRateSchedule(Schedule& schedule) {
      bool result =
        setSchedule(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName, "PumpVariableSpeedCondensate", "Pump Flow Rate", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetPumpFlowRateSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpCurve(const Curve& curve) {
      return setPointer(OS_Pump_VariableSpeed_CondensateFields::PumpCurveName, curve.handle());
    }

    void PumpVariableSpeedCondensate_Impl::resetPumpCurve() {
      bool ok = setString(OS_Pump_VariableSpeed_CondensateFields::PumpCurveName, "");
      OS_ASSERT(ok);
    }

    bool PumpVariableSpeedCondensate_Impl::setImpellerDiameter(boost::optional<double> impellerDiameter) {
      bool result(false);
      if (impellerDiameter) {
        result = setDouble(OS_Pump_VariableSpeed_CondensateFields::ImpellerDiameter, impellerDiameter.get());
      } else {
        resetImpellerDiameter();
        result = true;
      }
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetImpellerDiameter() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::ImpellerDiameter, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setVFDControlType(boost::optional<std::string> vFDControlType) {
      bool result(false);
      if (vFDControlType) {
        result = setString(OS_Pump_VariableSpeed_CondensateFields::VFDControlType, vFDControlType.get());
      } else {
        resetVFDControlType();
        result = true;
      }
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetVFDControlType() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::VFDControlType, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpRPMSchedule(Schedule& schedule) {
      bool result = setSchedule(OS_Pump_VariableSpeed_CondensateFields::PumpRPMScheduleName, "PumpVariableSpeedCondensate", "Pump RPM", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetPumpRPMSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::PumpRPMScheduleName, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMinimumPressureSchedule(Schedule& schedule) {
      bool result =
        setSchedule(OS_Pump_VariableSpeed_CondensateFields::MinimumPressureSchedule, "PumpVariableSpeedCondensate", "Minimum Pressure", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMinimumPressureSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MinimumPressureSchedule, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMaximumPressureSchedule(Schedule& schedule) {
      bool result =
        setSchedule(OS_Pump_VariableSpeed_CondensateFields::MaximumPressureSchedule, "PumpVariableSpeedCondensate", "Maximum Pressure", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMaximumPressureSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MaximumPressureSchedule, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMinimumRPMSchedule(Schedule& schedule) {
      bool result = setSchedule(OS_Pump_VariableSpeed_CondensateFields::MinimumRPMSchedule, "PumpVariableSpeedCondensate", "Minimum RPM", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMinimumRPMSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MinimumRPMSchedule, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::setMaximumRPMSchedule(Schedule& schedule) {
      bool result = setSchedule(OS_Pump_VariableSpeed_CondensateFields::MaximumRPMSchedule, "PumpVariableSpeedCondensate", "Maximum RPM", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetMaximumRPMSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::MaximumRPMSchedule, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::addToNode(Node& node) {
      if (node.plantLoop()) {
        return StraightComponent_Impl::addToNode(node);
      }

      return false;
    }

    std::vector<std::string> PumpVariableSpeedCondensate_Impl::pumpControlTypeValues() const {
      return PumpVariableSpeedCondensate::pumpControlTypeValues();
    }

    std::vector<std::string> PumpVariableSpeedCondensate_Impl::vfdControlTypeValues() const {
      return PumpVariableSpeedCondensate::vfdControlTypeValues();
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::pumpFlowRateScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = pumpFlowRateSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::pumpCurveAsModelObject() const {
      OptionalModelObject result;
      OptionalCurve intermediate = pumpCurve();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::pumpRPMScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = pumpRPMSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::minimumPressureScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = minimumPressureSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::maximumPressureScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = maximumPressureSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::minimumRPMScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = minimumRPMSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::maximumRPMScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = maximumRPMSchedule();
      if (intermediate) {
        result = *intermediate;
      }
      return result;
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpFlowRateScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setPumpFlowRateSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetPumpFlowRateSchedule();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpCurveAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalCurve intermediate = modelObject->optionalCast<Curve>();
        if (intermediate) {
          return setPumpCurve(*intermediate);
        } else {
          return false;
        }
      } else {
        resetPumpCurve();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setPumpRPMScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setPumpRPMSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetPumpRPMSchedule();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setMinimumPressureScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setMinimumPressureSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetMinimumPressureSchedule();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setMaximumPressureScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setMaximumPressureSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetMaximumPressureSchedule();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setMinimumRPMScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setMinimumRPMSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetMinimumRPMSchedule();
      }
      return true;
    }

    bool PumpVariableSpeedCondensate_Impl::setMaximumRPMScheduleAsModelObject(const boost::optional<ModelObject>& modelObject) {
      if (modelObject) {
        OptionalSchedule intermediate = modelObject->optionalCast<Schedule>();
        if (intermediate) {
          Schedule schedule(*intermediate);
          return setMaximumRPMSchedule(schedule);
        } else {
          return false;
        }
      } else {
        resetMaximumRPMSchedule();
      }
      return true;
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::autosizedRatedFlowRate() const {
      return getAutosizedValue("Design Flow Rate", "m3/s");
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::autosizedRatedPowerConsumption() const {
      return getAutosizedValue("Design Power Consumption", "W");
    }

    void PumpVariableSpeedCondensate_Impl::autosize() {
      autosizeRatedFlowRate();
      autosizeRatedPowerConsumption();
    }

    void PumpVariableSpeedCondensate_Impl::applySizingValues() {
      boost::optional<double> val;
      val = autosizedRatedFlowRate();
      if (val) {
        setRatedFlowRate(val.get());
      }

      val = autosizedRatedPowerConsumption();
      if (val) {
        setRatedPowerConsumption(val.get());
      }
    }

    std::string PumpVariableSpeedCondensate_Impl::designPowerSizingMethod() const {
      auto value = getString(OS_Pump_VariableSpeed_CondensateFields::DesignPowerSizingMethod, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setDesignPowerSizingMethod(const std::string& designPowerSizingMethod) {
      return setString(OS_Pump_VariableSpeed_CondensateFields::DesignPowerSizingMethod, designPowerSizingMethod);
    }

    double PumpVariableSpeedCondensate_Impl::designElectricPowerPerUnitFlowRate() const {
      auto value = getDouble(OS_Pump_VariableSpeed_CondensateFields::DesignElectricPowerperUnitFlowRate, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setDesignElectricPowerPerUnitFlowRate(double designElectricPowerPerUnitFlowRate) {
      return setDouble(OS_Pump_VariableSpeed_CondensateFields::DesignElectricPowerperUnitFlowRate, designElectricPowerPerUnitFlowRate);
    }

    double PumpVariableSpeedCondensate_Impl::designShaftPowerPerUnitFlowRatePerUnitHead() const {
      auto value = getDouble(OS_Pump_VariableSpeed_CondensateFields::DesignShaftPowerperUnitFlowRateperUnitHead, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setDesignShaftPowerPerUnitFlowRatePerUnitHead(double designShaftPowerPerUnitFlowRatePerUnitHead) {
      return setDouble(OS_Pump_VariableSpeed_CondensateFields::DesignShaftPowerperUnitFlowRateperUnitHead,
                       designShaftPowerPerUnitFlowRatePerUnitHead);
    }

    double PumpVariableSpeedCondensate_Impl::designMinimumFlowRateFraction() const {
      auto value = getDouble(OS_Pump_VariableSpeed_CondensateFields::DesignMinimumFlowRateFraction, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setDesignMinimumFlowRateFraction(double designMinimumFlowRateFraction) {
      return setDouble(OS_Pump_VariableSpeed_CondensateFields::DesignMinimumFlowRateFraction, designMinimumFlowRateFraction);
    }

    double PumpVariableSpeedCondensate_Impl::skinLossRadiativeFraction() const {
      auto value = getDouble(OS_Pump_VariableSpeed_CondensateFields::SkinLossRadiativeFraction, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setSkinLossRadiativeFraction(double skinLossRadiativeFraction) {
      return setDouble(OS_Pump_VariableSpeed_CondensateFields::SkinLossRadiativeFraction, skinLossRadiativeFraction);
    }

    boost::optional<ThermalZone> PumpVariableSpeedCondensate_Impl::zone() const {
      return getObject<ModelObject>().getModelObjectTarget<ThermalZone>(OS_Pump_VariableSpeed_CondensateFields::ZoneName);
    }

    bool PumpVariableSpeedCondensate_Impl::setZone(const ThermalZone& thermalZone) {
      return setPointer(OS_Pump_VariableSpeed_CondensateFields::ZoneName, thermalZone.handle());
    }

    void PumpVariableSpeedCondensate_Impl::resetZone() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::ZoneName, "");
      OS_ASSERT(result);
    }

    std::vector<EMSActuatorNames> PumpVariableSpeedCondensate_Impl::emsActuatorNames() const {
      std::vector<EMSActuatorNames> actuators{{"Pump", "Pump Mass Flow Rate"}, {"Pump", "Pump Pressure Rise"}};
      return actuators;
    }

    std::vector<std::string> PumpVariableSpeedCondensate_Impl::emsInternalVariableNames() const {
      std::vector<std::string> types{"Pump Maximum Mass Flow Rate"};
      return types;
    }

    std::string PumpVariableSpeedCondensate_Impl::endUseSubcategory() const {
      auto value = getString(OS_Pump_VariableSpeed_CondensateFields::EndUseSubcategory, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool PumpVariableSpeedCondensate_Impl::setEndUseSubcategory(const std::string& endUseSubcategory) {
      return setString(OS_Pump_VariableSpeed_CondensateFields::EndUseSubcategory, endUseSubcategory);
    }

    ComponentType PumpVariableSpeedCondensate_Impl::componentType() const {
      return ComponentType::None;
    }

    std::vector<FuelType> PumpVariableSpeedCondensate_Impl::coolingFuelTypes() const {
      return {};
    }

    std::vector<FuelType> PumpVariableSpeedCondensate_Impl::heatingFuelTypes() const {
      return {};
    }

    std::vector<AppGFuelType> PumpVariableSpeedCondensate_Impl::appGHeatingFuelTypes() const {
      return {};
    }

  }  // namespace detail

  PumpVariableSpeedCondensate::PumpVariableSpeed(const Model& model) : StraightComponent(PumpVariableSpeedCondensate::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::PumpVariableSpeedCondensate_Impl>());

    setPumpControlType("Intermittent");

    setSkinLossRadiativeFraction(0.5);
    setDesignPowerSizingMethod("PowerPerFlowPerPressure");
    setDesignElectricPowerPerUnitFlowRate(348701.1);
    setDesignShaftPowerPerUnitFlowRatePerUnitHead(1.282051282);
    setDesignMinimumFlowRateFraction(0.0);

    setEndUseSubcategory("General");
  }

  IddObjectType PumpVariableSpeedCondensate::iddObjectType() {
    IddObjectType result(IddObjectType::OS_Pump_VariableSpeed);
    return result;
  }

  std::vector<std::string> PumpVariableSpeedCondensate::designPowerSizingMethodValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_Pump_VariableSpeed_CondensateFields::DesignPowerSizingMethod);
  }

  std::vector<std::string> PumpVariableSpeedCondensate::pumpControlTypeValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_Pump_VariableSpeed_CondensateFields::PumpControlType);
  }

  std::vector<std::string> PumpVariableSpeedCondensate::validPumpControlTypeValues() {
    return PumpVariableSpeedCondensate::pumpControlTypeValues();
  }

  std::vector<std::string> PumpVariableSpeedCondensate::vfdControlTypeValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_Pump_VariableSpeed_CondensateFields::VFDControlType);
  }

  boost::optional<double> PumpVariableSpeedCondensate::ratedFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->ratedFlowRate();
  }

  bool PumpVariableSpeedCondensate::isRatedFlowRateDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedFlowRateDefaulted();
  }

  bool PumpVariableSpeedCondensate::isRatedFlowRateAutosized() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedFlowRateAutosized();
  }

  double PumpVariableSpeedCondensate::ratedPumpHead() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->ratedPumpHead();
  }

  bool PumpVariableSpeedCondensate::isRatedPumpHeadDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedPumpHeadDefaulted();
  }

  boost::optional<double> PumpVariableSpeedCondensate::ratedPowerConsumption() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->ratedPowerConsumption();
  }

  bool PumpVariableSpeedCondensate::isRatedPowerConsumptionDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedPowerConsumptionDefaulted();
  }

  bool PumpVariableSpeedCondensate::isRatedPowerConsumptionAutosized() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedPowerConsumptionAutosized();
  }

  double PumpVariableSpeedCondensate::motorEfficiency() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->motorEfficiency();
  }

  bool PumpVariableSpeedCondensate::isMotorEfficiencyDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isMotorEfficiencyDefaulted();
  }

  double PumpVariableSpeedCondensate::fractionofMotorInefficienciestoFluidStream() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->fractionofMotorInefficienciestoFluidStream();
  }

  bool PumpVariableSpeedCondensate::isFractionofMotorInefficienciestoFluidStreamDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isFractionofMotorInefficienciestoFluidStreamDefaulted();
  }

  double PumpVariableSpeedCondensate::coefficient1ofthePartLoadPerformanceCurve() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->coefficient1ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::isCoefficient1ofthePartLoadPerformanceCurveDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isCoefficient1ofthePartLoadPerformanceCurveDefaulted();
  }

  double PumpVariableSpeedCondensate::coefficient2ofthePartLoadPerformanceCurve() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->coefficient2ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::isCoefficient2ofthePartLoadPerformanceCurveDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isCoefficient2ofthePartLoadPerformanceCurveDefaulted();
  }

  double PumpVariableSpeedCondensate::coefficient3ofthePartLoadPerformanceCurve() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->coefficient3ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::isCoefficient3ofthePartLoadPerformanceCurveDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isCoefficient3ofthePartLoadPerformanceCurveDefaulted();
  }

  double PumpVariableSpeedCondensate::coefficient4ofthePartLoadPerformanceCurve() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->coefficient4ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::isCoefficient4ofthePartLoadPerformanceCurveDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isCoefficient4ofthePartLoadPerformanceCurveDefaulted();
  }

  double PumpVariableSpeedCondensate::minimumFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->minimumFlowRate();
  }

  bool PumpVariableSpeedCondensate::isMinimumFlowRateDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isMinimumFlowRateDefaulted();
  }

  std::string PumpVariableSpeedCondensate::pumpControlType() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->pumpControlType();
  }

  bool PumpVariableSpeedCondensate::isPumpControlTypeDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isPumpControlTypeDefaulted();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::pumpFlowRateSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->pumpFlowRateSchedule();
  }

  boost::optional<Curve> PumpVariableSpeedCondensate::pumpCurve() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->pumpCurve();
  }

  boost::optional<double> PumpVariableSpeedCondensate::impellerDiameter() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->impellerDiameter();
  }

  boost::optional<std::string> PumpVariableSpeedCondensate::vFDControlType() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->vFDControlType();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::pumpRPMSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->pumpRPMSchedule();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::minimumPressureSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->minimumPressureSchedule();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::maximumPressureSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->maximumPressureSchedule();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::minimumRPMSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->minimumRPMSchedule();
  }

  boost::optional<Schedule> PumpVariableSpeedCondensate::maximumRPMSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->maximumRPMSchedule();
  }

  bool PumpVariableSpeedCondensate::setRatedFlowRate(double ratedFlowRate) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setRatedFlowRate(ratedFlowRate);
  }

  void PumpVariableSpeedCondensate::resetRatedFlowRate() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetRatedFlowRate();
  }

  void PumpVariableSpeedCondensate::autosizeRatedFlowRate() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizeRatedFlowRate();
  }

  bool PumpVariableSpeedCondensate::setRatedPumpHead(double ratedPumpHead) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setRatedPumpHead(ratedPumpHead);
  }

  void PumpVariableSpeedCondensate::resetRatedPumpHead() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetRatedPumpHead();
  }

  bool PumpVariableSpeedCondensate::setRatedPowerConsumption(double ratedPowerConsumption) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setRatedPowerConsumption(ratedPowerConsumption);
  }

  void PumpVariableSpeedCondensate::resetRatedPowerConsumption() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetRatedPowerConsumption();
  }

  void PumpVariableSpeedCondensate::autosizeRatedPowerConsumption() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizeRatedPowerConsumption();
  }

  bool PumpVariableSpeedCondensate::setMotorEfficiency(double motorEfficiency) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMotorEfficiency(motorEfficiency);
  }

  void PumpVariableSpeedCondensate::resetMotorEfficiency() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMotorEfficiency();
  }

  bool PumpVariableSpeedCondensate::setFractionofMotorInefficienciestoFluidStream(double fractionofMotorInefficienciestoFluidStream) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setFractionofMotorInefficienciestoFluidStream(
      fractionofMotorInefficienciestoFluidStream);
  }

  void PumpVariableSpeedCondensate::resetFractionofMotorInefficienciestoFluidStream() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetFractionofMotorInefficienciestoFluidStream();
  }

  bool PumpVariableSpeedCondensate::setCoefficient1ofthePartLoadPerformanceCurve(double coefficient1ofthePartLoadPerformanceCurve) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setCoefficient1ofthePartLoadPerformanceCurve(
      coefficient1ofthePartLoadPerformanceCurve);
  }

  void PumpVariableSpeedCondensate::resetCoefficient1ofthePartLoadPerformanceCurve() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetCoefficient1ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::setCoefficient2ofthePartLoadPerformanceCurve(double coefficient2ofthePartLoadPerformanceCurve) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setCoefficient2ofthePartLoadPerformanceCurve(
      coefficient2ofthePartLoadPerformanceCurve);
  }

  void PumpVariableSpeedCondensate::resetCoefficient2ofthePartLoadPerformanceCurve() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetCoefficient2ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::setCoefficient3ofthePartLoadPerformanceCurve(double coefficient3ofthePartLoadPerformanceCurve) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setCoefficient3ofthePartLoadPerformanceCurve(
      coefficient3ofthePartLoadPerformanceCurve);
  }

  void PumpVariableSpeedCondensate::resetCoefficient3ofthePartLoadPerformanceCurve() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetCoefficient3ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::setCoefficient4ofthePartLoadPerformanceCurve(double coefficient4ofthePartLoadPerformanceCurve) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setCoefficient4ofthePartLoadPerformanceCurve(
      coefficient4ofthePartLoadPerformanceCurve);
  }

  void PumpVariableSpeedCondensate::resetCoefficient4ofthePartLoadPerformanceCurve() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetCoefficient4ofthePartLoadPerformanceCurve();
  }

  bool PumpVariableSpeedCondensate::setMinimumFlowRate(double minimumFlowRate) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMinimumFlowRate(minimumFlowRate);
  }

  void PumpVariableSpeedCondensate::resetMinimumFlowRate() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMinimumFlowRate();
  }

  bool PumpVariableSpeedCondensate::setPumpControlType(const std::string& pumpControlType) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setPumpControlType(pumpControlType);
  }

  void PumpVariableSpeedCondensate::resetPumpControlType() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetPumpControlType();
  }

  bool PumpVariableSpeedCondensate::setPumpFlowRateSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setPumpFlowRateSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetPumpFlowRateSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetPumpFlowRateSchedule();
  }

  bool PumpVariableSpeedCondensate::setPumpCurve(const Curve& curve) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setPumpCurve(curve);
  }

  void PumpVariableSpeedCondensate::resetPumpCurve() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetPumpCurve();
  }

  bool PumpVariableSpeedCondensate::setImpellerDiameter(double impellerDiameter) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setImpellerDiameter(impellerDiameter);
  }

  void PumpVariableSpeedCondensate::resetImpellerDiameter() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetImpellerDiameter();
  }

  bool PumpVariableSpeedCondensate::setVFDControlType(const std::string& vFDControlType) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setVFDControlType(vFDControlType);
  }

  void PumpVariableSpeedCondensate::resetVFDControlType() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetVFDControlType();
  }

  bool PumpVariableSpeedCondensate::setPumpRPMSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setPumpRPMSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetPumpRPMSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetPumpRPMSchedule();
  }

  bool PumpVariableSpeedCondensate::setMinimumPressureSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMinimumPressureSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetMinimumPressureSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMinimumPressureSchedule();
  }

  bool PumpVariableSpeedCondensate::setMaximumPressureSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMaximumPressureSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetMaximumPressureSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMaximumPressureSchedule();
  }

  bool PumpVariableSpeedCondensate::setMinimumRPMSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMinimumRPMSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetMinimumRPMSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMinimumRPMSchedule();
  }

  bool PumpVariableSpeedCondensate::setMaximumRPMSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setMaximumRPMSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetMaximumRPMSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetMaximumRPMSchedule();
  }

  std::string PumpVariableSpeedCondensate::designPowerSizingMethod() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->designPowerSizingMethod();
  }

  bool PumpVariableSpeedCondensate::setDesignPowerSizingMethod(const std::string& designPowerSizingMethod) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setDesignPowerSizingMethod(designPowerSizingMethod);
  }

  double PumpVariableSpeedCondensate::designElectricPowerPerUnitFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->designElectricPowerPerUnitFlowRate();
  }

  bool PumpVariableSpeedCondensate::setDesignElectricPowerPerUnitFlowRate(double designElectricPowerPerUnitFlowRate) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setDesignElectricPowerPerUnitFlowRate(designElectricPowerPerUnitFlowRate);
  }

  double PumpVariableSpeedCondensate::designShaftPowerPerUnitFlowRatePerUnitHead() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->designShaftPowerPerUnitFlowRatePerUnitHead();
  }

  bool PumpVariableSpeedCondensate::setDesignShaftPowerPerUnitFlowRatePerUnitHead(double designShaftPowerPerUnitFlowRatePerUnitHead) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setDesignShaftPowerPerUnitFlowRatePerUnitHead(
      designShaftPowerPerUnitFlowRatePerUnitHead);
  }

  boost::optional<ThermalZone> PumpVariableSpeedCondensate::zone() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->zone();
  }

  bool PumpVariableSpeedCondensate::setZone(const ThermalZone& thermalZone) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setZone(thermalZone);
  }

  void PumpVariableSpeedCondensate::resetZone() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetZone();
  }

  double PumpVariableSpeedCondensate::skinLossRadiativeFraction() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->skinLossRadiativeFraction();
  }

  bool PumpVariableSpeedCondensate::setSkinLossRadiativeFraction(double skinLossRadiativeFraction) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setSkinLossRadiativeFraction(skinLossRadiativeFraction);
  }

  double PumpVariableSpeedCondensate::designMinimumFlowRateFraction() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->designMinimumFlowRateFraction();
  }

  bool PumpVariableSpeedCondensate::setDesignMinimumFlowRateFraction(double designMinimumFlowRateFraction) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setDesignMinimumFlowRateFraction(designMinimumFlowRateFraction);
  }

  std::string PumpVariableSpeedCondensate::endUseSubcategory() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->endUseSubcategory();
  }

  bool PumpVariableSpeedCondensate::setEndUseSubcategory(const std::string& endUseSubcategory) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setEndUseSubcategory(endUseSubcategory);
  }

  /// @cond
  PumpVariableSpeedCondensate::PumpVariableSpeed(std::shared_ptr<detail::PumpVariableSpeedCondensate_Impl> impl)
    : StraightComponent(std::move(impl)) {}
  /// @endcond

  boost::optional<double> PumpVariableSpeedCondensate::autosizedRatedFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizedRatedFlowRate();
  }

  boost::optional<double> PumpVariableSpeedCondensate::autosizedRatedPowerConsumption() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizedRatedPowerConsumption();
  }

}  // namespace model
}  // namespace openstudio
