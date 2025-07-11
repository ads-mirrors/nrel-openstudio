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
      return result;
    }

    IddObjectType PumpVariableSpeedCondensate_Impl::iddObjectType() const {
      return PumpVariableSpeedCondensate::iddObjectType();
    }

    std::vector<ModelObject> PumpVariableSpeedCondensate_Impl::children() const {
      ModelObjectVector result;

      return result;
    }

    unsigned PumpVariableSpeedCondensate_Impl::inletPort() const {
      return OS_Pump_VariableSpeed_CondensateFields::InletNodeName;
    }

    unsigned PumpVariableSpeedCondensate_Impl::outletPort() const {
      return OS_Pump_VariableSpeed_CondensateFields::OutletNodeName;
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::ratedSteamVolumeFlowRate() const {
      return getDouble(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate, true);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedSteamVolumeFlowRateDefaulted() const {
      return isEmpty(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate);
    }

    bool PumpVariableSpeedCondensate_Impl::isRatedSteamVolumeFlowRateAutosized() const {
      bool result = false;
      boost::optional<std::string> value = getString(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate, true);
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

    boost::optional<Schedule> PumpVariableSpeedCondensate_Impl::pumpFlowRateSchedule() const {
      return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName);
    }

    bool PumpVariableSpeedCondensate_Impl::setRatedSteamVolumeFlowRate(boost::optional<double> ratedSteamVolumeFlowRate) {
      bool result(false);
      if (ratedSteamVolumeFlowRate) {
        result = setDouble(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate, ratedSteamVolumeFlowRate.get());
      } else {
        resetRatedSteamVolumeFlowRate();
        result = true;
      }
      OS_ASSERT(result);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetRatedSteamVolumeFlowRate() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate, "");
      OS_ASSERT(result);
    }

    void PumpVariableSpeedCondensate_Impl::autosizeRatedSteamVolumeFlowRate() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::RatedSteamVolumeFlowRate, "autosize");
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

    bool PumpVariableSpeedCondensate_Impl::setPumpFlowRateSchedule(Schedule& schedule) {
      bool result =
        setSchedule(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName, "PumpVariableSpeedCondensate", "Pump Flow Rate", schedule);
      return result;
    }

    void PumpVariableSpeedCondensate_Impl::resetPumpFlowRateSchedule() {
      bool result = setString(OS_Pump_VariableSpeed_CondensateFields::PumpFlowRateScheduleName, "");
      OS_ASSERT(result);
    }

    bool PumpVariableSpeedCondensate_Impl::addToNode(Node& node) {
      if (node.plantLoop()) {
        return StraightComponent_Impl::addToNode(node);
      }

      return false;
    }

    boost::optional<ModelObject> PumpVariableSpeedCondensate_Impl::pumpFlowRateScheduleAsModelObject() const {
      OptionalModelObject result;
      OptionalSchedule intermediate = pumpFlowRateSchedule();
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

    boost::optional<double> PumpVariableSpeedCondensate_Impl::autosizedRatedSteamVolumeFlowRate() const {
      return getAutosizedValue("Design Steam Volume Flow Rate", "m3/s");
    }

    boost::optional<double> PumpVariableSpeedCondensate_Impl::autosizedRatedPowerConsumption() const {
      return getAutosizedValue("Design Power Consumption", "W");
    }

    void PumpVariableSpeedCondensate_Impl::autosize() {
      autosizeRatedSteamVolumeFlowRate();
      autosizeRatedPowerConsumption();
    }

    void PumpVariableSpeedCondensate_Impl::applySizingValues() {
      boost::optional<double> val;
      val = autosizedRatedSteamVolumeFlowRate();
      if (val) {
        setRatedSteamVolumeFlowRate(val.get());
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

  PumpVariableSpeedCondensate::PumpVariableSpeedCondensate(const Model& model)
    : StraightComponent(PumpVariableSpeedCondensate::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::PumpVariableSpeedCondensate_Impl>());

    setSkinLossRadiativeFraction(0.5);
    setDesignPowerSizingMethod("PowerPerFlowPerPressure");
    setDesignElectricPowerPerUnitFlowRate(348701.1);
    setDesignShaftPowerPerUnitFlowRatePerUnitHead(1.282051282);

    setEndUseSubcategory("General");
  }

  IddObjectType PumpVariableSpeedCondensate::iddObjectType() {
    IddObjectType result(IddObjectType::OS_Pump_VariableSpeed);
    return result;
  }

  std::vector<std::string> PumpVariableSpeedCondensate::designPowerSizingMethodValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_Pump_VariableSpeed_CondensateFields::DesignPowerSizingMethod);
  }

  boost::optional<double> PumpVariableSpeedCondensate::ratedSteamVolumeFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->ratedSteamVolumeFlowRate();
  }

  bool PumpVariableSpeedCondensate::isRatedSteamVolumeFlowRateDefaulted() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedSteamVolumeFlowRateDefaulted();
  }

  bool PumpVariableSpeedCondensate::isRatedSteamVolumeFlowRateAutosized() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->isRatedSteamVolumeFlowRateAutosized();
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

  boost::optional<Schedule> PumpVariableSpeedCondensate::pumpFlowRateSchedule() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->pumpFlowRateSchedule();
  }

  bool PumpVariableSpeedCondensate::setRatedSteamVolumeFlowRate(double ratedSteamVolumeFlowRate) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setRatedSteamVolumeFlowRate(ratedSteamVolumeFlowRate);
  }

  void PumpVariableSpeedCondensate::resetRatedSteamVolumeFlowRate() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetRatedSteamVolumeFlowRate();
  }

  void PumpVariableSpeedCondensate::autosizeRatedSteamVolumeFlowRate() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizeRatedSteamVolumeFlowRate();
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

  bool PumpVariableSpeedCondensate::setPumpFlowRateSchedule(Schedule& schedule) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setPumpFlowRateSchedule(schedule);
  }

  void PumpVariableSpeedCondensate::resetPumpFlowRateSchedule() {
    getImpl<detail::PumpVariableSpeedCondensate_Impl>()->resetPumpFlowRateSchedule();
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

  std::string PumpVariableSpeedCondensate::endUseSubcategory() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->endUseSubcategory();
  }

  bool PumpVariableSpeedCondensate::setEndUseSubcategory(const std::string& endUseSubcategory) {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->setEndUseSubcategory(endUseSubcategory);
  }

  /// @cond
  PumpVariableSpeedCondensate::PumpVariableSpeedCondensate(std::shared_ptr<detail::PumpVariableSpeedCondensate_Impl> impl)
    : StraightComponent(std::move(impl)) {}
  /// @endcond

  boost::optional<double> PumpVariableSpeedCondensate::autosizedRatedSteamVolumeFlowRate() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizedRatedSteamVolumeFlowRate();
  }

  boost::optional<double> PumpVariableSpeedCondensate::autosizedRatedPowerConsumption() const {
    return getImpl<detail::PumpVariableSpeedCondensate_Impl>()->autosizedRatedPowerConsumption();
  }

}  // namespace model
}  // namespace openstudio
