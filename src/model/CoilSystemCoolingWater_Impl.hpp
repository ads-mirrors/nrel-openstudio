/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_COILSYSTEMCOOLINGWATER_IMPL_HPP
#define MODEL_COILSYSTEMCOOLINGWATER_IMPL_HPP

#include <model/ModelAPI.hpp>
#include "StraightComponent_Impl.hpp"

namespace openstudio {
namespace model {

  class Schedule;
  class WaterToAirComponent;

  namespace detail {

    /** CoilSystemCoolingWater_Impl is a StraightComponent_Impl that is the implementation class for CoilSystemCoolingWater.*/
    class MODEL_API CoilSystemCoolingWater_Impl : public StraightComponent_Impl
    {
     public:
      /** @name Constructors and Destructors */
      //@{

      CoilSystemCoolingWater_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle);

      CoilSystemCoolingWater_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle);

      CoilSystemCoolingWater_Impl(const CoilSystemCoolingWater_Impl& other, Model_Impl* model, bool keepHandle);

      virtual ~CoilSystemCoolingWater_Impl() = default;

      //@}
      /** @name Virtual Methods */
      //@{

      virtual const std::vector<std::string>& outputVariableNames() const override;

      virtual IddObjectType iddObjectType() const override;

      virtual std::vector<ScheduleTypeKey> getScheduleTypeKeys(const Schedule& schedule) const override;

      virtual unsigned inletPort() const override;

      virtual unsigned outletPort() const override;

      // will return the coolingCoil and companionCoilUsedForHeatRecovery
      virtual std::vector<ModelObject> children() const override;

      // Will also clone the coolingCoil and companionCoilUsedForHeatRecovery
      virtual ModelObject clone(Model model) const override;

      // This function will connect the underlying Coil:Cooling:Water object
      virtual bool addToNode(Node& node) override;

      virtual boost::optional<HVACComponent> containingHVACComponent() const override;

      virtual boost::optional<ZoneHVACComponent> containingZoneHVACComponent() const override;

      virtual ComponentType componentType() const override;
      virtual std::vector<FuelType> coolingFuelTypes() const override;
      virtual std::vector<FuelType> heatingFuelTypes() const override;
      virtual std::vector<AppGFuelType> appGHeatingFuelTypes() const override;

      //@}
      /** @name Getters */
      //@{

      WaterToAirComponent coolingCoil() const;

      std::string dehumidificationControlType() const;

      bool runonSensibleLoad() const;

      bool runonLatentLoad() const;

      double minimumAirToWaterTemperatureOffset() const;

      bool economizerLockout() const;

      double minimumWaterLoopTemperatureForHeatRecovery() const;

      boost::optional<WaterToAirComponent> companionCoilUsedForHeatRecovery() const;

      //@}
      /** @name Setters */
      //@{

      bool setAvailabilitySchedule(Schedule& schedule);

      bool setCoolingCoil(const WaterToAirComponent& coolingCoil);

      bool setDehumidificationControlType(const std::string& dehumidificationControlType);

      bool setRunonSensibleLoad(bool runonSensibleLoad);

      bool setRunonLatentLoad(bool runonLatentLoad);

      bool setMinimumAirToWaterTemperatureOffset(double minimumAirToWaterTemperatureOffset);

      bool setEconomizerLockout(bool economizerLockout);

      bool setMinimumWaterLoopTemperatureForHeatRecovery(double minimumWaterLoopTemperatureForHeatRecovery);

      bool setCompanionCoilUsedForHeatRecovery(const WaterToAirComponent& companionCoilUsedForHeatRecovery);

      void resetCompanionCoilUsedForHeatRecovery();

      //@}
      /** @name Other */
      //@{

      //@}
     protected:
     private:
      REGISTER_LOGGER("openstudio.model.CoilSystemCoolingWater");

      boost::optional<Schedule> optionalAvailabilitySchedule() const;
      boost::optional<WaterToAirComponent> optionalCoolingCoil() const;
      boost::optional<WaterToAirComponent> optionalCompanionCoilUsedForHeatRecovery() const;
    };

  }  // namespace detail

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_COILSYSTEMCOOLINGWATER_IMPL_HPP
