/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "CoilSystemCoolingWater.hpp"
#include "CoilSystemCoolingWater_Impl.hpp"

// TODO: Check the following class names against object getters and setters.
#include "Connection.hpp"
#include "Connection_Impl.hpp"
#include "Schedule.hpp"
#include "Schedule_Impl.hpp"
#include "CoolingCoilsWater.hpp"
#include "CoolingCoilsWater_Impl.hpp"
#include "ScheduleTypeLimits.hpp"
#include "ScheduleTypeRegistry.hpp"

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
      // TODO: Check schedule display names.
      std::vector<ScheduleTypeKey> result;
      const UnsignedVector fieldIndices = getSourceIndices(schedule.handle());
      if (std::find(fieldIndices.cbegin(), fieldIndices.cend(), OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName) != fieldIndices.cend()) {
        result.emplace_back("CoilSystemCoolingWater", "Availability");
      }
      return result;
    }

    ComponentType CoilSystemCoolingWater_Impl::componentType() const {
      // TODO
      return ComponentType::None;
    }

    std::vector<FuelType> CoilSystemCoolingWater_Impl::coolingFuelTypes() const {
      // TODO
      return {};
    }

    std::vector<FuelType> CoilSystemCoolingWater_Impl::heatingFuelTypes() const {
      // TODO
      return {};
    }

    std::vector<AppGFuelType> CoilSystemCoolingWater_Impl::appGHeatingFuelTypes() const {

      // TODO
      return {};
    }

    Connection CoilSystemCoolingWater_Impl::airInletNode() const {
      boost::optional<Connection> value = optionalAirInletNode();
      if (!value) {
        LOG_AND_THROW(briefDescription() << " does not have an Air Inlet Node attached.");
      }
      return value.get();
    }

    Connection CoilSystemCoolingWater_Impl::airOutletNode() const {
      boost::optional<Connection> value = optionalAirOutletNode();
      if (!value) {
        LOG_AND_THROW(briefDescription() << " does not have an Air Outlet Node attached.");
      }
      return value.get();
    }

    Schedule CoilSystemCoolingWater_Impl::availabilitySchedule() const {
      boost::optional<Schedule> value = optionalAvailabilitySchedule();
      if (!value) {
        LOG_AND_THROW(briefDescription() << " does not have an Availability Schedule attached.");
      }
      return value.get();
    }

    CoolingCoilsWater CoilSystemCoolingWater_Impl::coolingCoil() const {
      boost::optional<CoolingCoilsWater> value = optionalCoolingCoil();
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

    boost::optional<CoolingCoilsWater> CoilSystemCoolingWater_Impl::companionCoilUsedForHeatRecovery() const {
      return getObject<ModelObject>().getModelObjectTarget<CoolingCoilsWater>(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery);
    }

    bool CoilSystemCoolingWater_Impl::setAirInletNode(const Connection& connection) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::AirInletNodeName, connection.handle());
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setAirOutletNode(const Connection& connection) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::AirOutletNodeName, connection.handle());
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setAvailabilitySchedule(Schedule& schedule) {
      const bool result =
        setSchedule(OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName, "CoilSystemCoolingWater", "Availability", schedule);
      return result;
    }

    bool CoilSystemCoolingWater_Impl::setCoolingCoil(const CoolingCoilsWater& coolingCoilsWater) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::CoolingCoil, coolingCoilsWater.handle());
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

    bool CoilSystemCoolingWater_Impl::setCompanionCoilUsedForHeatRecovery(const CoolingCoilsWater& coolingCoilsWater) {
      const bool result = setPointer(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, coolingCoilsWater.handle());
      return result;
    }

    void CoilSystemCoolingWater_Impl::resetCompanionCoilUsedForHeatRecovery() {
      const bool result = setString(OS_CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, "");
      OS_ASSERT(result);
    }

    boost::optional<Connection> CoilSystemCoolingWater_Impl::optionalAirInletNode() const {
      return getObject<ModelObject>().getModelObjectTarget<Connection>(OS_CoilSystem_Cooling_WaterFields::AirInletNodeName);
    }

    boost::optional<Connection> CoilSystemCoolingWater_Impl::optionalAirOutletNode() const {
      return getObject<ModelObject>().getModelObjectTarget<Connection>(OS_CoilSystem_Cooling_WaterFields::AirOutletNodeName);
    }

    boost::optional<Schedule> CoilSystemCoolingWater_Impl::optionalAvailabilitySchedule() const {
      return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName);
    }

    boost::optional<CoolingCoilsWater> CoilSystemCoolingWater_Impl::optionalCoolingCoil() const {
      return getObject<ModelObject>().getModelObjectTarget<CoolingCoilsWater>(OS_CoilSystem_Cooling_WaterFields::CoolingCoil);
    }

  }  // namespace detail

  CoilSystemCoolingWater::CoilSystemCoolingWater(const Model& model) : StraightComponent(CoilSystemCoolingWater::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::CoilSystemCoolingWater_Impl>());

    // TODO: consider adding (overloaded or not) explicit ctors taking required objects as argument

    // TODO: Appropriately handle the following required object-list fields.
    //     OS_CoilSystem_Cooling_WaterFields::AirInletNodeName
    //     OS_CoilSystem_Cooling_WaterFields::AirOutletNodeName
    //     OS_CoilSystem_Cooling_WaterFields::AvailabilityScheduleName
    //     OS_CoilSystem_Cooling_WaterFields::CoolingCoil
    bool ok = true;
    // ok = setAirInletNode();
    OS_ASSERT(ok);
    // ok = setAirOutletNode();
    OS_ASSERT(ok);
    // ok = setAvailabilitySchedule();
    OS_ASSERT(ok);
    // ok = setCoolingCoil();
    OS_ASSERT(ok);
    // ok = setDehumidificationControlType();
    OS_ASSERT(ok);
    // setRunonSensibleLoad();
    // setRunonLatentLoad();
    // ok = setMinimumAirToWaterTemperatureOffset();
    OS_ASSERT(ok);
    // setEconomizerLockout();
    // setMinimumWaterLoopTemperatureForHeatRecovery();
  }

  IddObjectType CoilSystemCoolingWater::iddObjectType() {
    return {IddObjectType::OS_CoilSystem_Cooling_Water};
  }

  std::vector<std::string> CoilSystemCoolingWater::dehumidificationControlTypeValues() {
    return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(), OS_CoilSystem_Cooling_WaterFields::DehumidificationControlType);
  }

  Connection CoilSystemCoolingWater::airInletNode() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->airInletNode();
  }

  Connection CoilSystemCoolingWater::airOutletNode() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->airOutletNode();
  }

  Schedule CoilSystemCoolingWater::availabilitySchedule() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->availabilitySchedule();
  }

  CoolingCoilsWater CoilSystemCoolingWater::coolingCoil() const {
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

  boost::optional<CoolingCoilsWater> CoilSystemCoolingWater::companionCoilUsedForHeatRecovery() const {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->companionCoilUsedForHeatRecovery();
  }

  bool CoilSystemCoolingWater::setAirInletNode(const Connection& connection) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setAirInletNode(connection);
  }

  bool CoilSystemCoolingWater::setAirOutletNode(const Connection& connection) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setAirOutletNode(connection);
  }

  bool CoilSystemCoolingWater::setAvailabilitySchedule(Schedule& schedule) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setAvailabilitySchedule(schedule);
  }

  bool CoilSystemCoolingWater::setCoolingCoil(const CoolingCoilsWater& coolingCoilsWater) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setCoolingCoil(coolingCoilsWater);
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

  bool CoilSystemCoolingWater::setCompanionCoilUsedForHeatRecovery(const CoolingCoilsWater& coolingCoilsWater) {
    return getImpl<detail::CoilSystemCoolingWater_Impl>()->setCompanionCoilUsedForHeatRecovery(coolingCoilsWater);
  }

  void CoilSystemCoolingWater::resetCompanionCoilUsedForHeatRecovery() {
    getImpl<detail::CoilSystemCoolingWater_Impl>()->resetCompanionCoilUsedForHeatRecovery();
  }

  /// @cond
  CoilSystemCoolingWater::CoilSystemCoolingWater(std::shared_ptr<detail::CoilSystemCoolingWater_Impl> impl) : StraightComponent(std::move(impl)) {}
  /// @endcond

}  // namespace model
}  // namespace openstudio
