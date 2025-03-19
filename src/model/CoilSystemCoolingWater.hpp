/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_COILSYSTEMCOOLINGWATER_HPP
#define MODEL_COILSYSTEMCOOLINGWATER_HPP

#include <model/ModelAPI.hpp>
#include "StraightComponent.hpp"

namespace openstudio {

namespace model {

  // TODO: Check the following class names against object getters and setters.
  class Connection;
  class Schedule;
  class CoolingCoilsWater;

  namespace detail {

    class CoilSystemCoolingWater_Impl;

  }  // namespace detail

  /** CoilSystemCoolingWater is a StraightComponent that wraps the OpenStudio IDD object 'OS:CoilSystem:Cooling:Water'. */
  class MODEL_API CoilSystemCoolingWater : public StraightComponent
  {
   public:
    /** @name Constructors and Destructors */
    //@{

    explicit CoilSystemCoolingWater(const Model& model);

    virtual ~CoilSystemCoolingWater() = default;
    // Default the copy and move operators because the virtual dtor is explicit
    CoilSystemCoolingWater(const CoilSystemCoolingWater& other) = default;
    CoilSystemCoolingWater(CoilSystemCoolingWater&& other) = default;
    CoilSystemCoolingWater& operator=(const CoilSystemCoolingWater&) = default;
    CoilSystemCoolingWater& operator=(CoilSystemCoolingWater&&) = default;

    //@}

    static IddObjectType iddObjectType();

    static std::vector<std::string> dehumidificationControlTypeValues();

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
    /// @cond
    using ImplType = detail::CoilSystemCoolingWater_Impl;

    explicit CoilSystemCoolingWater(std::shared_ptr<detail::CoilSystemCoolingWater_Impl> impl);

    friend class detail::CoilSystemCoolingWater_Impl;
    friend class Model;
    friend class IdfObject;
    friend class openstudio::detail::IdfObject_Impl;
    /// @endcond
   private:
    REGISTER_LOGGER("openstudio.model.CoilSystemCoolingWater");
  };

  /** \relates CoilSystemCoolingWater*/
  using OptionalCoilSystemCoolingWater = boost::optional<CoilSystemCoolingWater>;

  /** \relates CoilSystemCoolingWater*/
  using CoilSystemCoolingWaterVector = std::vector<CoilSystemCoolingWater>;

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_COILSYSTEMCOOLINGWATER_HPP
