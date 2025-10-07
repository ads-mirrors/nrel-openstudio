/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_HEATPUMPAIRTOWATERCOOLINGSPEEDDATA_HPP
#define MODEL_HEATPUMPAIRTOWATERCOOLINGSPEEDDATA_HPP

#include "ModelAPI.hpp"
#include "ParentObject.hpp"

namespace openstudio {

namespace model {

  class Curve;

  namespace detail {

    class HeatPumpAirToWaterCoolingSpeedData_Impl;

  }  // namespace detail

  /** HeatPumpAirToWaterCoolingSpeedData is a ParentObject that wraps the OpenStudio IDD object 'OS:HeatPump:AirToWater:Cooling:SpeedData'. */
  class MODEL_API HeatPumpAirToWaterCoolingSpeedData : public ParentObject
  {
   public:
    /** @name Constructors and Destructors */
    //@{

    explicit HeatPumpAirToWaterCoolingSpeedData(const Model& model);

    explicit HeatPumpAirToWaterCoolingSpeedData(const Model& model, const Curve& normalizedCoolingCapacityFunctionofTemperatureCurve,
                                                const Curve& coolingEnergyInputRatioFunctionofTemperatureCurve,
                                                const Curve& coolingEnergyInputRatioFunctionofPLRCurve);

    virtual ~HeatPumpAirToWaterCoolingSpeedData() = default;
    // Default the copy and move operators because the virtual dtor is explicit
    HeatPumpAirToWaterCoolingSpeedData(const HeatPumpAirToWaterCoolingSpeedData& other) = default;
    HeatPumpAirToWaterCoolingSpeedData(HeatPumpAirToWaterCoolingSpeedData&& other) = default;
    HeatPumpAirToWaterCoolingSpeedData& operator=(const HeatPumpAirToWaterCoolingSpeedData&) = default;
    HeatPumpAirToWaterCoolingSpeedData& operator=(HeatPumpAirToWaterCoolingSpeedData&&) = default;

    //@}

    static IddObjectType iddObjectType();

    /** @name Getters */
    //@{

    boost::optional<double> ratedCoolingCapacity() const;

    bool isRatedCoolingCapacityAutosized() const;

    double ratedCOPforCooling() const;

    Curve normalizedCoolingCapacityFunctionofTemperatureCurve() const;

    Curve coolingEnergyInputRatioFunctionofTemperatureCurve() const;

    Curve coolingEnergyInputRatioFunctionofPLRCurve() const;

    //@}
    /** @name Setters */
    //@{

    bool setRatedCoolingCapacity(double ratedCoolingCapacity);

    void autosizeRatedCoolingCapacity();

    bool setRatedCOPforCooling(double ratedCOPforCooling);

    bool setNormalizedCoolingCapacityFunctionofTemperatureCurve(const Curve& bivariateFunctions);

    bool setCoolingEnergyInputRatioFunctionofTemperatureCurve(const Curve& bivariateFunctions);

    bool setCoolingEnergyInputRatioFunctionofPLRCurve(const Curve& univariateFunctions);

    //@}
    /** @name Other */
    //@{

    // returns a vector of HeatPumpAirToWater that use this as their Speed Data
    // TODO: std::vector<HeatPumpAirToWater> heatPumpsAirToWater() const;

    boost::optional<double> autosizedRatedCoolingCapacity();

    void autosize();

    //@}
   protected:
    /// @cond
    using ImplType = detail::HeatPumpAirToWaterCoolingSpeedData_Impl;

    explicit HeatPumpAirToWaterCoolingSpeedData(std::shared_ptr<detail::HeatPumpAirToWaterCoolingSpeedData_Impl> impl);

    friend class detail::HeatPumpAirToWaterCoolingSpeedData_Impl;
    friend class Model;
    friend class IdfObject;
    friend class openstudio::detail::IdfObject_Impl;
    /// @endcond
   private:
    REGISTER_LOGGER("openstudio.model.HeatPumpAirToWaterCoolingSpeedData");
  };

  /** \relates HeatPumpAirToWaterCoolingSpeedData*/
  using OptionalHeatPumpAirToWaterCoolingSpeedData = boost::optional<HeatPumpAirToWaterCoolingSpeedData>;

  /** \relates HeatPumpAirToWaterCoolingSpeedData*/
  using HeatPumpAirToWaterCoolingSpeedDataVector = std::vector<HeatPumpAirToWaterCoolingSpeedData>;

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_HEATPUMPAIRTOWATERCOOLINGSPEEDDATA_HPP
