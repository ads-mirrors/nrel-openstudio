/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "CoilSystemCoolingWater.hpp"
#include "CoilSystemCoolingWater_Impl.hpp"

#include "Schedule.hpp"
#include "Schedule_Impl.hpp"
#include "WaterToAirComponent.hpp"
#include "WaterToAirComponent_Impl.hpp"
#include "CoilCoolingWater.hpp"
#include "CoilCoolingWater_Impl.hpp"
#include "Model.hpp"
#include "Model_Impl.hpp"
#include "Node.hpp"
#include "Node_Impl.hpp"
#include "AirLoopHVAC.hpp"
#include "AirLoopHVAC_Impl.hpp"
#include "ControllerWaterCoil.hpp"
#include "ControllerWaterCoil_Impl.hpp"

#include "AirLoopHVACUnitarySystem.hpp"
#include "AirLoopHVACUnitarySystem_Impl.hpp"
#include "ZoneHVACFourPipeFanCoil.hpp"
#include "ZoneHVACFourPipeFanCoil_Impl.hpp"
#include "ZoneHVACUnitVentilator.hpp"
#include "ZoneHVACUnitVentilator_Impl.hpp"

#include "AirLoopHVACOutdoorAirSystem.hpp"
#include "AirLoopHVACOutdoorAirSystem_Impl.hpp"
#include "AirLoopHVACDedicatedOutdoorAirSystem.hpp"
#include "AirLoopHVACDedicatedOutdoorAirSystem_Impl.hpp"

#include "../utilities/core/Assert.hpp"
#include "../utilities/data/DataEnums.hpp"

#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/OS_CoilSystem_Cooling_Water_FieldEnums.hxx>

namespace openstudio {
namespace model {

  namespace detail {

    CoilSystemCoolingWater_Impl::CoilSystemCoolingWater_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(idfObject, model, keepHandle) {
      OS_ASSERT(idfObject.iddObject().type() == CoilSystemCoolingWater::iddObjectType());
    }

    CoilSystemCoolingWater_Impl::CoilSystemCoolingWater_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model,
                                                             bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {
      OS_ASSERT(other.iddObject().type() == CoilSystemCoolingWater::iddObjectType());
    }

    CoilSystemCoolingWater_Impl::CoilSystemCoolingWater_Impl(const CoilSystemCoolingWater_Impl& other, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {}

    const std::vector<std::string>& CoilSystemCoolingWater_Impl::outputVariableNames() const {
      static std::vector<std::string> result;
      if (result.empty()) {
      }
      return result;
    }

    IddObjectType CoilSystemCoolingWater_Impl::iddObjectType() const {
      return CoilSystemCoolingWater::iddObjectType();
    }

    std::vector<ScheduleTypeKey> CoilSystemCoolingWater_Impl::getScheduleTypeKeys(const Schedule& schedule) const {
      std::vector<ScheduleTypeKey> result;
      const UnsignedVector fieldIndices = getSourceIndices(schedule.handle());
      if (std::find(fieldIndices.cbegin(), fieldIndices.cend(), OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName) != fieldIndices.cend()) {
        result.emplace_back("CoilSystemCoolingWater", "Availability");
      }
      return result;
    }

    unsigned CoilSystemCoolingWater_Impl::inletPort() const {
      return OS_CoilSystem_Cooling_WaterFields::AirInletNodeName;
    }

    unsigned CoilSystemCoolingWater_Impl::outletPort() const {
      return OS_CoilSystem_Cooling_WaterFields::AirOutletNodeName;
    }

    std::vector<ModelObject> CoilSystemCoolingWater_Impl::children() const {
      std::vector<ModelObject> result;
      if (OptionalWaterToAirComponent intermediate = optionalCoolingCoil()) {
        result.push_back(*intermediate);
      }
      if (OptionalWaterToAirComponent intermediate = optionalCompanionCoilUsedForHeatRecovery()) {
        result.push_back(*intermediate);
      }

      return result;
    }

    ModelObject CoilSystemCoolingWater_Impl::clone(Model model) const {
      auto coilSystemClone = StraightComponent_Impl::clone(model).cast<CoilSystemCoolingWater>();

      if (OptionalWaterToAirComponent intermediate = optionalCoolingCoil()) {
        coilSystemClone.setCoolingCoil(intermediate->clone(model).cast<WaterToAirComponent>());
      }
      if (OptionalWaterToAirComponent intermediate = optionalCompanionCoilUsedForHeatRecovery()) {
        coilSystemClone.setCompanionCoilUsedForHeatRecovery(intermediate->clone(model).cast<WaterToAirComponent>());
      }

      return std::move(coilSystemClone);
    }

    bool CoilSystemCoolingWater_Impl::addToNode(Node& node) {
      bool result = false;

      if (boost::optional<AirLoopHVAC> airLoop = node.airLoopHVAC()) {
        if (!airLoop->demandComponent(node.handle())) {
          result = StraightComponent_Impl::addToNode(node);
          if (result) {
            auto t_coolingCoil = coolingCoil();
            if (auto waterInletModelObject = t_coolingCoil.waterInletModelObject()) {

              // TODO: why aren't we setting the water coil in this case? @kbenne thoughts please
              if (auto coilCoolingWater = t_coolingCoil.optionalCast<CoilCoolingWater>()) {
                if (auto oldController = coilCoolingWater->controllerWaterCoil()) {
                  oldController->remove();
                }
              }

              /*               auto t_model = model();
              ControllerWaterCoil controller(t_model);

              auto coilWaterInletNode = waterInletModelObject->optionalCast<Node>();
              OS_ASSERT(coilWaterInletNode);
              controller.setActuatorNode(coilWaterInletNode.get());
              // sensor node will be established in translator since that node does not yet exist

              controller.setAction("Reverse"); */
            }
          }
        }
      } else if (boost::optional<AirLoopHVACOutdoorAirSystem> oas = node.airLoopHVACOutdoorAirSystem()) {
        if (oas->airLoopHVACDedicatedOutdoorAirSystem()) {
          return StraightComponent_Impl::addToNode(node);
        }
      }

      return result;
    }

    boost::optional<HVACComponent> CoilSystemCoolingWater_Impl::containingHVACComponent() const {
      // AirLoopHVACUnitarySystem
      std::vector<AirLoopHVACUnitarySystem> airLoopHVACUnitarySystems = this->model().getConcreteModelObjects<AirLoopHVACUnitarySystem>();

      for (const auto& airLoopHVACUnitarySystem : airLoopHVACUnitarySystems) {
        if (boost::optional<HVACComponent> coolingCoil = airLoopHVACUnitarySystem.coolingCoil()) {
          if (coolingCoil->handle() == this->handle()) {
            return airLoopHVACUnitarySystem;
          }
        }
      }

      return boost::none;
    }

    boost::optional<ZoneHVACComponent> CoilSystemCoolingWater_Impl::containingZoneHVACComponent() const {

      // ZoneHVACFourPipeFanCoil
      std::vector<ZoneHVACFourPipeFanCoil> zoneHVACFourPipeFanCoils;

      zoneHVACFourPipeFanCoils = this->model().getConcreteModelObjects<ZoneHVACFourPipeFanCoil>();

      for (const auto& zoneHVACFourPipeFanCoil : zoneHVACFourPipeFanCoils) {
        if (boost::optional<HVACComponent> coil = zoneHVACFourPipeFanCoil.coolingCoil()) {
          if (coil->handle() == this->handle()) {
            return zoneHVACFourPipeFanCoil;
          }
        }
      }

      // ZoneHVACUnitVentilator
      std::vector<ZoneHVACUnitVentilator> zoneHVACUnitVentilators;

      zoneHVACUnitVentilators = this->model().getConcreteModelObjects<ZoneHVACUnitVentilator>();

      for (const auto& zoneHVACUnitVentilator : zoneHVACUnitVentilators) {
        if (boost::optional<HVACComponent> coil = zoneHVACUnitVentilator.coolingCoil()) {
          if (coil->handle() == this->handle()) {
            return zoneHVACUnitVentilator;
          }
        }
      }

      // ZoneHVAC:WindowAirConditioner not wrapped

      return boost::none;
    }

    ComponentType CoilSystemCoolingWater_Impl::componentType() const {
      return ComponentType::Cooling;
    }

    std::vector<FuelType> CoilSystemCoolingWater_Impl::coolingFuelTypes() const {
      return coolingCoil().coolingFuelTypes();
    }

    std::vector<FuelType> CoilSystemCoolingWater_Impl::heatingFuelTypes() const {
      return {};
    }

    std::vector<AppGFuelType> CoilSystemCoolingWater_Impl::appGHeatingFuelTypes() const {
      return {};
    }

    Schedule CoilSystemCoolingWater_Impl::availabilitySchedule() const {
      boost::optional<Schedule> value = optionalAvailabilitySchedule();
      if (!value) {
        LOG_AND_THROW(briefDescription() << " does not have an Availability Schedule attached.");
      }
      return value.get();
    }

    WaterToAirComponent CoilSystemCoolingWater_Impl::coolingCoil() const {
      boost::optional<WaterToAirComponent> value = optionalCoolingCoil();
      if (!value) {
        LOG_AND_THROW(briefDescription() << " does not have an Cooling Coil attached.");
      }
      return value.get();
    }

    std::string CoilSystemCoolingWater_Impl::dehumidificationControlType() const {
      boost::optional<std::string> value = getString(OS_CoilSystem_Cooling_WaterFields::DehumidificationControlType, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool CoilSystemCoolingWater_Impl::runonSensibleLoad() const {
      return getBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::RunonSensibleLoad);
    }

    bool CoilSystemCoolingWater_Impl::runonLatentLoad() const {
      return getBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::RunonLatentLoad);
    }

    double CoilSystemCoolingWater_Impl::minimumAirToWaterTemperatureOffset() const {
      boost::optional<double> value = getDouble(OS_CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool CoilSystemCoolingWater_Impl::economizerLockout() const {
      return getBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::EconomizerLockout);
    }

    double CoilSystemCoolingWater_Impl::minimumWaterLoopTemperatureForHeatRecovery() const {
      boost::optional<double> value = getDouble(OS_CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery, true);
      OS_ASSERT(value);
      return value.get();
    }

    boost::optional<WaterToAirComponent> CoilSystemCoolingWater_Impl::companionCoilUsedForHeatRecovery() const {
      return optionalCompanionCoilUsedForHeatRecovery();
    }

    bool CoilSystemCoolingWater_Impl::setAvailabilitySchedule(Schedule& schedule) {
      const bool result =
        setSchedule(OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName, "CoilSystemCoolingWater", "Availability", schedule);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setCoolingCoil(const WaterToAirComponent& coolingCoil) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::CoolingCoil, coolingCoil.handle());
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setDehumidificationControlType(const std::string& dehumidificationControlType) {
      const bool result = setString(OS_CoilSystem_Cooling_WaterFields::DehumidificationControlType, dehumidificationControlType);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setRunonSensibleLoad(bool runonSensibleLoad) {
      const bool result = setBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::RunonSensibleLoad, runonSensibleLoad);
      OS_ASSERT(result);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setRunonLatentLoad(bool runonLatentLoad) {
      const bool result = setBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::RunonLatentLoad, runonLatentLoad);
      OS_ASSERT(result);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setMinimumAirToWaterTemperatureOffset(double minimumAirToWaterTemperatureOffset) {
      const bool result = setDouble(OS_CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset, minimumAirToWaterTemperatureOffset);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setEconomizerLockout(bool economizerLockout) {
      const bool result = setBooleanFieldValue(OS_CoilSystem_Cooling_WaterFields::EconomizerLockout, economizerLockout);
      OS_ASSERT(result);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setMinimumWaterLoopTemperatureForHeatRecovery(double minimumWaterLoopTemperatureForHeatRecovery) {
      const bool result =
        setDouble(OS_CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery, minimumWaterLoopTemperatureForHeatRecovery);
      OS_ASSERT(result);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setCompanionCoilUsedForHeatRecovery(const WaterToAirComponent& companionCoilUsedForHeatRecovery) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, companionCoilUsedForHeatRecovery.handle());
      return result;
    }

    void CoilSystemCoolingWater_Impl::resetCompanionCoilUsedForHeatRecovery() {
      const bool result = setString(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, "");
      OS_ASSERT(result);
    }

    boost::optional<Schedule> CoilSystemCoolingWater_Impl::optionalAvailabilitySchedule() const {
      return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName);
    }

    boost::optional<WaterToAirComponent> CoilSystemCoolingWater_Impl::optionalCoolingCoil() const {
      return getObject<ModelObject>().getModelObjectTarget<WaterToAirComponent>(OS_CoilSystem_Cooling_WaterFields::CoolingCoil);
    }

    boost::optional<WaterToAirComponent> CoilSystemCoolingWater_Impl::optionalCompanionCoilUsedForHeatRecovery() const {
      return getObject<ModelObject>().getModelObjectTarget<WaterToAirComponent>(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery);
    }

  }  // namespace detail

  CoilSystemCoolingWater::CoilSystemCoolingWater(const Model& model) : StraightComponent(CoilSystemCoolingWater::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::CoilSystemCoolingWater_Impl>());

    bool ok = true;
    auto alwaysOn = model.alwaysOnDiscreteSchedule();
    ok = setAvailabilitySchedule(alwaysOn);
    OS_ASSERT(ok);

    CoilCoolingWater coolingCoil(model);
    ok = setCoolingCoil(coolingCoil);
    OS_ASSERT(ok);
    ok = setDehumidificationControlType("None");
    OS_ASSERT(ok);
    ok = setRunonSensibleLoad(true);
    OS_ASSERT(ok);
    ok = setRunonLatentLoad(false);
    OS_ASSERT(ok);
    ok = setMinimumAirToWaterTemperatureOffset(0.0);
    OS_ASSERT(ok);
    ok = setEconomizerLockout(true);
    OS_ASSERT(ok);
    ok = setMinimumWaterLoopTemperatureForHeatRecovery(0.0);
    OS_ASSERT(ok);
  }

  CoilSystemCoolingWater::CoilSystemCoolingWater(const Model& model, const WaterToAirComponent& coolingCoil)
    : StraightComponent(CoilSystemCoolingWater::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::CoilSystemCoolingWater_Impl>());

    bool ok = true;
    auto alwaysOn = model.alwaysOnDiscreteSchedule();
    ok = setAvailabilitySchedule(alwaysOn);
    OS_ASSERT(ok);

    ok = setCoolingCoil(coolingCoil);
    OS_ASSERT(ok);
    ok = setDehumidificationControlType("None");
    OS_ASSERT(ok);
    ok = setRunonSensibleLoad(true);
    OS_ASSERT(ok);
    ok = setRunonLatentLoad(false);
    OS_ASSERT(ok);
    ok = setMinimumAirToWaterTemperatureOffset(0.0);
    OS_ASSERT(ok);
    ok = setEconomizerLockout(true);
    OS_ASSERT(ok);
    ok = setMinimumWaterLoopTemperatureForHeatRecovery(0.0);
    OS_ASSERT(ok);
  }

  IddObjectType CoilSystemCoolingWater::iddObjectType() {
    return {IddObjectType::OS_CoilSystem_Cooling_Water};
  }

  std::vector<std::string> CoilSystemCoolingWater::dehumidificationControlTypeValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_CoilSystem_Cooling_WaterFields::DehumidificationControlType);
  }

  Schedule CoilSystemCoolingWater::availabilitySchedule() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->availabilitySchedule();
  }

  WaterToAirComponent CoilSystemCoolingWater::coolingCoil() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->coolingCoil();
  }

  std::string CoilSystemCoolingWater::dehumidificationControlType() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->dehumidificationControlType();
  }

  bool CoilSystemCoolingWater::runonSensibleLoad() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->runonSensibleLoad();
  }

  bool CoilSystemCoolingWater::runonLatentLoad() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->runonLatentLoad();
  }

  double CoilSystemCoolingWater::minimumAirToWaterTemperatureOffset() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->minimumAirToWaterTemperatureOffset();
  }

  bool CoilSystemCoolingWater::economizerLockout() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->economizerLockout();
  }

  double CoilSystemCoolingWater::minimumWaterLoopTemperatureForHeatRecovery() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->minimumWaterLoopTemperatureForHeatRecovery();
  }

  boost::optional<WaterToAirComponent> CoilSystemCoolingWater::companionCoilUsedForHeatRecovery() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->companionCoilUsedForHeatRecovery();
  }

  bool CoilSystemCoolingWater::setAvailabilitySchedule(Schedule& schedule) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setAvailabilitySchedule(schedule);
  }

  bool CoilSystemCoolingWater::setCoolingCoil(const WaterToAirComponent& coolingCoil) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setCoolingCoil(coolingCoil);
  }

  bool CoilSystemCoolingWater::setDehumidificationControlType(const std::string& dehumidificationControlType) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setDehumidificationControlType(dehumidificationControlType);
  }

  bool CoilSystemCoolingWater::setRunonSensibleLoad(bool runonSensibleLoad) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setRunonSensibleLoad(runonSensibleLoad);
  }

  bool CoilSystemCoolingWater::setRunonLatentLoad(bool runonLatentLoad) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setRunonLatentLoad(runonLatentLoad);
  }

  bool CoilSystemCoolingWater::setMinimumAirToWaterTemperatureOffset(double minimumAirToWaterTemperatureOffset) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setMinimumAirToWaterTemperatureOffset(minimumAirToWaterTemperatureOffset);
  }

  bool CoilSystemCoolingWater::setEconomizerLockout(bool economizerLockout) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setEconomizerLockout(economizerLockout);
  }

  bool CoilSystemCoolingWater::setMinimumWaterLoopTemperatureForHeatRecovery(double minimumWaterLoopTemperatureForHeatRecovery) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setMinimumWaterLoopTemperatureForHeatRecovery(minimumWaterLoopTemperatureForHeatRecovery);
  }

  bool CoilSystemCoolingWater::setCompanionCoilUsedForHeatRecovery(const WaterToAirComponent& companionCoilUsedForHeatRecovery) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery);
  }

  void CoilSystemCoolingWater::resetCompanionCoilUsedForHeatRecovery() {
    getImpl<detail::CoilSystemCoolingWater_Impl>()->resetCompanionCoilUsedForHeatRecovery();
  }

  /// @cond
  CoilSystemCoolingWater::CoilSystemCoolingWater(std::shared_ptr<detail::CoilSystemCoolingWater_Impl> impl) : StraightComponent(std::move(impl)) {}
  /// @endcond

}  // namespace model
}  // namespace openstudio
