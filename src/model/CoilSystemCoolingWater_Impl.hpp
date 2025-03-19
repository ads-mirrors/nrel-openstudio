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

  // TODO: Check the following class names against object getters and setters.
  class Connection;
  class Schedule;
  class CoolingCoilsWater;

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

      virtual ComponentType componentType() const override;
      virtual std::vector<FuelType> coolingFuelTypes() const override;
      virtual std::vector<FuelType> heatingFuelTypes() const override;
      virtual std::vector<AppGFuelType> appGHeatingFuelTypes() const override;

      //@}
      /** @name Getters */
      //@{

      // TODO: Check return type. From object lists, some candidates are: Connection.
      Connection airInletNode() const;

      // TODO: Check return type. From object lists, some candidates are: Connection.
      Connection airOutletNode() const;

      // TODO: Check return type. From object lists, some candidates are: Schedule.
      Schedule availabilitySchedule() const;

      // TODO: Check return type. From object lists, some candidates are: CoolingCoilsWater.
      CoolingCoilsWater coolingCoil() const;

      std::string dehumidificationControlType() const;

      bool runonSensibleLoad() const;

      bool runonLatentLoad() const;

      double minimumAirToWaterTemperatureOffset() const;

      bool economizerLockout() const;

      double minimumWaterLoopTemperatureForHeatRecovery() const;

      // TODO: Check return type. From object lists, some candidates are: CoolingCoilsWater.
      boost::optional<CoolingCoilsWater> companionCoilUsedForHeatRecovery() const;

      //@}
      /** @name Setters */
      //@{

      // TODO: Check argument type. From object lists, some candidates are: Connection.
      bool setAirInletNode(const Connection& connection);

      // TODO: Check argument type. From object lists, some candidates are: Connection.
      bool setAirOutletNode(const Connection& connection);

      // TODO: Check argument type. From object lists, some candidates are: Schedule.
      // Note Schedules are passed by reference, not const reference.
      bool setAvailabilitySchedule(Schedule& schedule);

      // TODO: Check argument type. From object lists, some candidates are: CoolingCoilsWater.
      bool setCoolingCoil(const CoolingCoilsWater& coolingCoilsWater);

      bool setDehumidificationControlType(const std::string& dehumidificationControlType);

      bool setRunonSensibleLoad(bool runonSensibleLoad);

      bool setRunonLatentLoad(bool runonLatentLoad);

      bool setMinimumAirToWaterTemperatureOffset(double minimumAirToWaterTemperatureOffset);

      bool setEconomizerLockout(bool economizerLockout);

      bool setMinimumWaterLoopTemperatureForHeatRecovery(double minimumWaterLoopTemperatureForHeatRecovery);

      // TODO: Check argument type. From object lists, some candidates are: CoolingCoilsWater.
      bool setCompanionCoilUsedForHeatRecovery(const CoolingCoilsWater& coolingCoilsWater);

      void resetCompanionCoilUsedForHeatRecovery();

      //@}
      /** @name Other */
      //@{

      //@}
     protected:
     private:
      REGISTER_LOGGER("openstudio.model.CoilSystemCoolingWater");

      // TODO: Check the return types of these methods.
      // Optional getters for use by methods like children() so can remove() if the constructor fails.
      // There are other ways for the public versions of these getters to fail--perhaps all required
      // objects should be returned as boost::optionals
      boost::optional<Connection> optionalAirInletNode() const;
      boost::optional<Connection> optionalAirOutletNode() const;
      boost::optional<Schedule> optionalAvailabilitySchedule() const;
      boost::optional<CoolingCoilsWater> optionalCoolingCoil() const;
    };

  }  // namespace detail

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_COILSYSTEMCOOLINGWATER_IMPL_HPP
