/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_PUMPVARIABLESPEEDCONDENSATE_HPP
#define MODEL_PUMPVARIABLESPEEDCONDENSATE_HPP

#include "ModelAPI.hpp"
#include "StraightComponent.hpp"

namespace openstudio {
namespace model {

  class Schedule;
  class Curve;
  class ThermalZone;

  namespace detail {

    class PumpVariableSpeedCondensate_Impl;

  }  // namespace detail

  /** PumpVariableSpeedCondensate is a StraightComponent that wraps the OpenStudio IDD object
 *  'OS:Pump:VariableSpeed:Condensate'. */
  class MODEL_API PumpVariableSpeedCondensate : public StraightComponent
  {
   public:
    /** @name Constructors and Destructors */
    //@{

    explicit PumpVariableSpeedCondensate(const Model& model);

    virtual ~PumpVariableSpeedCondensate() override = default;
    // Default the copy and move operators because the virtual dtor is explicit
    PumpVariableSpeedCondensate(const PumpVariableSpeedCondensate& other) = default;
    PumpVariableSpeedCondensate(PumpVariableSpeedCondensate&& other) = default;
    PumpVariableSpeedCondensate& operator=(const PumpVariableSpeedCondensate&) = default;
    PumpVariableSpeedCondensate& operator=(PumpVariableSpeedCondensate&&) = default;

    //@}

    static IddObjectType iddObjectType();

    static std::vector<std::string> designPowerSizingMethodValues();

    /** @name Getters */
    //@{

    boost::optional<double> ratedSteamVolumeFlowRate() const;

    bool isRatedSteamVolumeFlowRateDefaulted() const;

    bool isRatedSteamVolumeFlowRateAutosized() const;

    double ratedPumpHead() const;

    bool isRatedPumpHeadDefaulted() const;

    boost::optional<double> ratedPowerConsumption() const;

    bool isRatedPowerConsumptionDefaulted() const;

    bool isRatedPowerConsumptionAutosized() const;

    double motorEfficiency() const;

    bool isMotorEfficiencyDefaulted() const;

    double fractionofMotorInefficienciestoFluidStream() const;

    bool isFractionofMotorInefficienciestoFluidStreamDefaulted() const;

    double coefficient1ofthePartLoadPerformanceCurve() const;

    bool isCoefficient1ofthePartLoadPerformanceCurveDefaulted() const;

    double coefficient2ofthePartLoadPerformanceCurve() const;

    bool isCoefficient2ofthePartLoadPerformanceCurveDefaulted() const;

    double coefficient3ofthePartLoadPerformanceCurve() const;

    bool isCoefficient3ofthePartLoadPerformanceCurveDefaulted() const;

    double coefficient4ofthePartLoadPerformanceCurve() const;

    bool isCoefficient4ofthePartLoadPerformanceCurveDefaulted() const;

    boost::optional<Schedule> pumpFlowRateSchedule() const;

    std::string endUseSubcategory() const;

    //@}
    /** @name Setters */
    //@{

    bool setRatedSteamVolumeFlowRate(double ratedSteamVolumeFlowRate);

    void resetRatedSteamVolumeFlowRate();

    void autosizeRatedSteamVolumeFlowRate();

    bool setRatedPumpHead(double ratedPumpHead);

    void resetRatedPumpHead();

    bool setRatedPowerConsumption(double ratedPowerConsumption);

    void resetRatedPowerConsumption();

    void autosizeRatedPowerConsumption();

    bool setMotorEfficiency(double motorEfficiency);

    void resetMotorEfficiency();

    bool setFractionofMotorInefficienciestoFluidStream(double fractionofMotorInefficienciestoFluidStream);

    void resetFractionofMotorInefficienciestoFluidStream();

    bool setCoefficient1ofthePartLoadPerformanceCurve(double coefficient1ofthePartLoadPerformanceCurve);

    void resetCoefficient1ofthePartLoadPerformanceCurve();

    bool setCoefficient2ofthePartLoadPerformanceCurve(double coefficient2ofthePartLoadPerformanceCurve);

    void resetCoefficient2ofthePartLoadPerformanceCurve();

    bool setCoefficient3ofthePartLoadPerformanceCurve(double coefficient3ofthePartLoadPerformanceCurve);

    void resetCoefficient3ofthePartLoadPerformanceCurve();

    bool setCoefficient4ofthePartLoadPerformanceCurve(double coefficient4ofthePartLoadPerformanceCurve);

    void resetCoefficient4ofthePartLoadPerformanceCurve();

    bool setPumpFlowRateSchedule(Schedule& schedule);

    void resetPumpFlowRateSchedule();

    boost::optional<double> autosizedRatedSteamVolumeFlowRate() const;

    boost::optional<double> autosizedRatedPowerConsumption() const;

    std::string designPowerSizingMethod() const;

    bool setDesignPowerSizingMethod(const std::string& designPowerSizingMethod);

    double designElectricPowerPerUnitFlowRate() const;

    bool setDesignElectricPowerPerUnitFlowRate(double designElectricPowerPerUnitFlowRate);

    double designShaftPowerPerUnitFlowRatePerUnitHead() const;

    bool setDesignShaftPowerPerUnitFlowRatePerUnitHead(double designShaftPowerPerUnitFlowRatePerUnitHead);

    boost::optional<ThermalZone> zone() const;

    bool setZone(const ThermalZone& thermalZone);

    void resetZone();

    double skinLossRadiativeFraction() const;

    bool setSkinLossRadiativeFraction(double skinLossRadiativeFraction);

    bool setEndUseSubcategory(const std::string& endUseSubcategory);

    //@}
   protected:
    /// @cond
    using ImplType = detail::PumpVariableSpeedCondensate_Impl;

    friend class detail::PumpVariableSpeedCondensate_Impl;
    friend class Model;
    friend class IdfObject;
    friend class openstudio::detail::IdfObject_Impl;

    explicit PumpVariableSpeedCondensate(std::shared_ptr<detail::PumpVariableSpeedCondensate_Impl> impl);

    /// @endcond
   private:
    REGISTER_LOGGER("openstudio.model.PumpVariableSpeedCondensate");
  };

  /** \relates PumpVariableSpeedCondensate*/
  using OptionalPumpVariableSpeed = boost::optional<PumpVariableSpeedCondensate>;

  /** \relates PumpVariableSpeedCondensate*/
  using PumpVariableSpeedCondensateVector = std::vector<PumpVariableSpeedCondensate>;

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_PUMPVARIABLESPEEDCONDENSATE_HPP
