/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_PUMPVARIABLESPEEDCONDENSATE_IMPL_HPP
#define MODEL_PUMPVARIABLESPEEDCONDENSATE_IMPL_HPP

#include "ModelAPI.hpp"
#include "StraightComponent_Impl.hpp"

namespace openstudio {
namespace model {

  class Schedule;
  class Curve;

  namespace detail {

    /** PumpVariableSpeedCondensate_Impl is a StraightComponent_Impl that is the implementation class for
   *  PumpVariableSpeedCondensate.*/
    class MODEL_API PumpVariableSpeedCondensate_Impl : public StraightComponent_Impl
    {
     public:
      /** @name Constructors and Destructors */
      //@{

      PumpVariableSpeedCondensate_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle);

      PumpVariableSpeedCondensate_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle);

      PumpVariableSpeedCondensate_Impl(const PumpVariableSpeedCondensate_Impl& other, Model_Impl* model, bool keepHandle);

      virtual ~PumpVariableSpeedCondensate_Impl() override = default;

      //@}
      /** @name Virtual Methods */
      //@{

      virtual const std::vector<std::string>& outputVariableNames() const override;

      virtual IddObjectType iddObjectType() const override;

      virtual std::vector<ScheduleTypeKey> getScheduleTypeKeys(const Schedule& schedule) const override;

      virtual std::vector<ModelObject> children() const override;

      virtual unsigned inletPort() const override;

      virtual unsigned outletPort() const override;

      virtual bool addToNode(Node& node) override;

      virtual void autosize() override;

      virtual void applySizingValues() override;

      virtual std::vector<EMSActuatorNames> emsActuatorNames() const override;

      virtual std::vector<std::string> emsInternalVariableNames() const override;

      virtual ComponentType componentType() const override;
      virtual std::vector<FuelType> coolingFuelTypes() const override;
      virtual std::vector<FuelType> heatingFuelTypes() const override;
      virtual std::vector<AppGFuelType> appGHeatingFuelTypes() const override;

      //@}
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

      boost::optional<double> autosizedRatedSteamVolumeFlowRate() const;

      boost::optional<double> autosizedRatedPowerConsumption() const;

      std::string endUseSubcategory() const;

      //@}
      /** @name Setters */
      //@{

      bool setRatedSteamVolumeFlowRate(boost::optional<double> ratedSteamVolumeFlowRate);

      void resetRatedSteamVolumeFlowRate();

      void autosizeRatedSteamVolumeFlowRate();

      bool setRatedPumpHead(double ratedPumpHead);

      void resetRatedPumpHead();

      bool setRatedPowerConsumption(boost::optional<double> ratedPowerConsumption);

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
     private:
      REGISTER_LOGGER("openstudio.model.PumpVariableSpeedCondensate");

      std::vector<std::string> pumpControlTypeValues() const;
      std::vector<std::string> vfdControlTypeValues() const;

      boost::optional<ModelObject> pumpFlowRateScheduleAsModelObject() const;

      bool setPumpFlowRateScheduleAsModelObject(const boost::optional<ModelObject>& modelObject);
    };

  }  // namespace detail

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_PUMPVARIABLESPEEDCONDENSATE_IMPL_HPP
