/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../HeatPumpAirToWaterHeating.hpp"
#include "../HeatPumpAirToWaterHeating_Impl.hpp"

#include "../HeatPumpAirToWater.hpp"
#include "../HeatPumpAirToWater_Impl.hpp"
#include "../HeatPumpAirToWaterHeatingSpeedData.hpp"
#include "../HeatPumpAirToWaterHeatingSpeedData_Impl.hpp"
#include "../HeatPumpAirToWaterCooling.hpp"

#include "../Model.hpp"
#include "../Schedule.hpp"
#include "../ScheduleConstant.hpp"
#include "../Curve.hpp"
#include "../Curve_Impl.hpp"
#include "../CurveCubic.hpp"
#include "../CurveBicubic.hpp"

#include "../AirLoopHVAC.hpp"
#include "../PlantLoop.hpp"
#include "../Node.hpp"
#include "../Node_Impl.hpp"
#include "../Splitter.hpp"

#include "../ModelObjectList.hpp"
#include "../ModelObjectList_Impl.hpp"

#include <utilities/idd/IddEnums.hxx>

#include <algorithm>
#include <fmt/format.h>
#include <fmt/ranges.h>

using namespace openstudio;
using namespace openstudio::model;

std::string getObjectNames(const auto& rmed) {
  std::vector<std::string> rm_names;
  rm_names.reserve(rmed.size());
  std::transform(rmed.cbegin(), rmed.cend(), std::back_inserter(rm_names), [](const auto& idfObjet) { return idfObjet.nameString(); });
  return fmt::format("Removed objects: {}", rm_names);
};

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_GettersSetters) {
  Model m;
  HeatPumpAirToWaterHeating awhp(m);

  awhp.setName("My HeatPumpAirToWaterHeating");

  // Availability Schedule Name: Required Object
  // Ctor default: Always On
  EXPECT_EQ(m.alwaysOnDiscreteSchedule(), awhp.availabilitySchedule());
  ScheduleConstant availabilitySchedule(m);
  EXPECT_TRUE(awhp.setAvailabilitySchedule(availabilitySchedule));
  EXPECT_EQ(availabilitySchedule, awhp.availabilitySchedule());

  // Rated Inlet Air Temperature: Required Double
  // Ctor default: 8.0
  EXPECT_EQ(8.0, awhp.ratedInletAirTemperature());
  EXPECT_TRUE(awhp.setRatedInletAirTemperature(0.4));
  EXPECT_EQ(0.4, awhp.ratedInletAirTemperature());

  // Rated Air Flow Rate: Required Double
  // Ctor default: Autosize
  EXPECT_TRUE(awhp.isRatedAirFlowRateAutosized());
  // Set
  EXPECT_TRUE(awhp.setRatedAirFlowRate(0.5));
  ASSERT_TRUE(awhp.ratedAirFlowRate());
  EXPECT_EQ(0.5, awhp.ratedAirFlowRate().get());
  // Bad Value
  EXPECT_FALSE(awhp.setRatedAirFlowRate(-10.0));
  ASSERT_TRUE(awhp.ratedAirFlowRate());
  EXPECT_EQ(0.5, awhp.ratedAirFlowRate().get());
  EXPECT_FALSE(awhp.isRatedAirFlowRateAutosized());
  // Autosize
  awhp.autosizeRatedAirFlowRate();
  EXPECT_TRUE(awhp.isRatedAirFlowRateAutosized());

  // Rated Leaving Water Temperature: Required Double
  // Ctor default: 40.0
  EXPECT_EQ(40.0, awhp.ratedLeavingWaterTemperature());
  EXPECT_TRUE(awhp.setRatedLeavingWaterTemperature(0.6));
  EXPECT_EQ(0.6, awhp.ratedLeavingWaterTemperature());

  // Rated Water Flow Rate: Required Double
  // Ctor default: Autosize
  EXPECT_TRUE(awhp.isRatedWaterFlowRateAutosized());
  // Set
  EXPECT_TRUE(awhp.setRatedWaterFlowRate(0.7));
  ASSERT_TRUE(awhp.ratedWaterFlowRate());
  EXPECT_EQ(0.7, awhp.ratedWaterFlowRate().get());
  // Bad Value
  EXPECT_FALSE(awhp.setRatedWaterFlowRate(-10.0));
  ASSERT_TRUE(awhp.ratedWaterFlowRate());
  EXPECT_EQ(0.7, awhp.ratedWaterFlowRate().get());
  EXPECT_FALSE(awhp.isRatedWaterFlowRateAutosized());
  // Autosize
  awhp.autosizeRatedWaterFlowRate();
  EXPECT_TRUE(awhp.isRatedWaterFlowRateAutosized());

  // Minimum Outdoor Air Temperature: Required Double
  // Ctor default: -30.0
  EXPECT_EQ(-30.0, awhp.minimumOutdoorAirTemperature());
  EXPECT_TRUE(awhp.setMinimumOutdoorAirTemperature(0.8));
  EXPECT_EQ(0.8, awhp.minimumOutdoorAirTemperature());

  // Maximum Outdoor Air Temperature: Required Double
  // Ctor default: 100.0
  EXPECT_EQ(100.0, awhp.maximumOutdoorAirTemperature());
  EXPECT_TRUE(awhp.setMaximumOutdoorAirTemperature(0.9));
  EXPECT_EQ(0.9, awhp.maximumOutdoorAirTemperature());

  // Minimum Leaving Water Temperature Curve Name: Optional Object
  CurveCubic minimumLeavingWaterTemperatureCurve(m);
  EXPECT_TRUE(awhp.setMinimumLeavingWaterTemperatureCurve(minimumLeavingWaterTemperatureCurve));
  ASSERT_TRUE(awhp.minimumLeavingWaterTemperatureCurve());
  EXPECT_EQ(minimumLeavingWaterTemperatureCurve, awhp.minimumLeavingWaterTemperatureCurve().get());
  awhp.resetMinimumLeavingWaterTemperatureCurve();
  EXPECT_FALSE(awhp.minimumLeavingWaterTemperatureCurve());

  // Maximum Leaving Water Temperature Curve Name: Optional Object
  CurveCubic maximumLeavingWaterTemperatureCurve(m);
  EXPECT_TRUE(awhp.setMaximumLeavingWaterTemperatureCurve(maximumLeavingWaterTemperatureCurve));
  ASSERT_TRUE(awhp.maximumLeavingWaterTemperatureCurve());
  EXPECT_EQ(maximumLeavingWaterTemperatureCurve, awhp.maximumLeavingWaterTemperatureCurve().get());

  // Sizing Factor: Required Double
  // Ctor default: 1.0
  EXPECT_EQ(1.0, awhp.sizingFactor());
  // Set
  EXPECT_TRUE(awhp.setSizingFactor(1.2));
  EXPECT_EQ(1.2, awhp.sizingFactor());
  // Bad Value
  EXPECT_FALSE(awhp.setSizingFactor(-10.0));
  EXPECT_EQ(1.2, awhp.sizingFactor());

  // Booster Mode On Speed: Optional Object
  HeatPumpAirToWaterHeatingSpeedData boosterModeOnSpeed(m);
  EXPECT_TRUE(awhp.setBoosterModeOnSpeed(boosterModeOnSpeed));
  ASSERT_TRUE(awhp.boosterModeOnSpeed());
  EXPECT_EQ(boosterModeOnSpeed, awhp.boosterModeOnSpeed().get());
  awhp.resetBoosterModeOnSpeed();
  EXPECT_FALSE(awhp.boosterModeOnSpeed());
}

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_HeatCoolFuelTypes) {
  Model m;
  HeatPumpAirToWaterHeating awhp(m);

  EXPECT_EQ(ComponentType(ComponentType::Heating), awhp.componentType());
  testFuelTypeEquality({}, awhp.coolingFuelTypes());
  testFuelTypeEquality({FuelType::Electricity}, awhp.heatingFuelTypes());
  testAppGFuelTypeEquality({AppGFuelType::HeatPump}, awhp.appGHeatingFuelTypes());
}

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_Speeds) {

  // create a model to use
  Model model;

  HeatPumpAirToWaterHeating awhp(model);

  std::vector<HeatPumpAirToWaterHeatingSpeedData> speeds;
  EXPECT_EQ(5, HeatPumpAirToWaterHeating::maximum_number_of_speeds);
  for (unsigned i = 1; i <= HeatPumpAirToWaterHeating::maximum_number_of_speeds; ++i) {
    HeatPumpAirToWaterHeatingSpeedData speed(model);
    speed.setName("Speed " + std::to_string(i));
    speeds.push_back(speed);
    EXPECT_TRUE(awhp.addSpeed(speed));
    EXPECT_EQ(i, awhp.numberOfSpeeds());
    EXPECT_EQ(speeds, awhp.speeds());
  }

  // Can't add more than 5 Speeds
  HeatPumpAirToWaterHeatingSpeedData anotherSpeed(model);
  EXPECT_FALSE(awhp.addSpeed(anotherSpeed));
  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  // Can't remove a speed that's not in there...
  EXPECT_FALSE(awhp.removeSpeed(anotherSpeed));
  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  {
    int speedIndex = 3;
    std::vector<HeatPumpAirToWaterHeatingSpeedData> thisSpeeds = awhp.speeds();
    // Explicit copy, so we can keep using it after it's been removed
    const auto speedAtIndex = thisSpeeds[speedIndex - 1];
    EXPECT_TRUE(std::find(thisSpeeds.begin(), thisSpeeds.end(), speedAtIndex) != thisSpeeds.end());
    auto optIndex = awhp.speedIndex(speedAtIndex);
    ASSERT_TRUE(optIndex);
    EXPECT_EQ(speedIndex, optIndex.get());
    EXPECT_TRUE(awhp.removeSpeed(speedIndex));
    EXPECT_EQ(4, awhp.numberOfSpeeds());
    thisSpeeds = awhp.speeds();
    EXPECT_FALSE(std::find(thisSpeeds.begin(), thisSpeeds.end(), speedAtIndex) != thisSpeeds.end());
    // Do the same on our vector, so we're up to date...
    speeds.erase(speeds.begin() + speedIndex - 1);
    EXPECT_EQ(speeds, awhp.speeds());
  }

  {
    int speedIndex = 2;
    std::vector<HeatPumpAirToWaterHeatingSpeedData> thisSpeeds = awhp.speeds();
    // Explicit copy, so we can keep using it after it's been removed
    const auto speedAtIndex = thisSpeeds[speedIndex - 1];
    EXPECT_TRUE(std::find(thisSpeeds.begin(), thisSpeeds.end(), speedAtIndex) != thisSpeeds.end());
    auto optIndex = awhp.speedIndex(speedAtIndex);
    ASSERT_TRUE(optIndex);
    EXPECT_EQ(speedIndex, optIndex.get());
    EXPECT_TRUE(awhp.removeSpeed(speedAtIndex));
    EXPECT_EQ(3, awhp.numberOfSpeeds());
    thisSpeeds = awhp.speeds();
    EXPECT_FALSE(std::find(thisSpeeds.begin(), thisSpeeds.end(), speedAtIndex) != thisSpeeds.end());
    // Do the same on our vector, so we're up to date...
    speeds.erase(std::find(speeds.begin(), speeds.end(), speedAtIndex));
    EXPECT_EQ(speeds, awhp.speeds());
  }

  {
    const auto& speedAtEnd = speeds.back();
    auto optIndex = awhp.speedIndex(speedAtEnd);
    ASSERT_TRUE(optIndex);
    EXPECT_EQ(awhp.numberOfSpeeds(), optIndex.get());

    EXPECT_TRUE(awhp.setSpeedIndex(speedAtEnd, 2));
    std::vector<HeatPumpAirToWaterHeatingSpeedData> thisSpeeds = awhp.speeds();
    optIndex = awhp.speedIndex(speedAtEnd);
    ASSERT_TRUE(optIndex);
    EXPECT_EQ(2, optIndex.get());
    EXPECT_EQ(3, awhp.numberOfSpeeds());
    for (unsigned i = 1; i <= awhp.numberOfSpeeds(); ++i) {
      if (i < optIndex.get()) {
        EXPECT_EQ(speeds[i - 1], awhp.speeds()[i - 1]);
      } else if (i > optIndex.get()) {
        EXPECT_EQ(speeds[i - 2], awhp.speeds()[i - 1]);
      }
    }
  }

  awhp.removeAllSpeeds();
  EXPECT_EQ(0, awhp.numberOfSpeeds());

  EXPECT_TRUE(awhp.setSpeeds(speeds));
  EXPECT_EQ(3, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  for (unsigned i = 6; i <= 8; ++i) {
    HeatPumpAirToWaterHeatingSpeedData speed(model);
    awhp.setName("Speed " + std::to_string(i));
    speeds.push_back(speed);
  }
  EXPECT_EQ(6, speeds.size());
  awhp.removeAllSpeeds();
  EXPECT_TRUE(awhp.addSpeed(anotherSpeed));

  // This should clear, then assign the first 5, but then return false since the 6th failed
  EXPECT_FALSE(awhp.setSpeeds(speeds));
  EXPECT_EQ(5, awhp.numberOfSpeeds());
  {
    std::vector<HeatPumpAirToWaterHeatingSpeedData> thisSpeeds = awhp.speeds();
    for (unsigned i = 0; i < 5; ++i) {
      EXPECT_EQ(speeds[i], thisSpeeds[i]);
    }
  }

  awhp.removeAllSpeeds();
  EXPECT_EQ(0, awhp.numberOfSpeeds());
  EXPECT_EQ(0, awhp.speeds().size());

  // Test that added a speed from another model will fail but not add a blank extensible group
  Model model2;
  HeatPumpAirToWaterHeatingSpeedData speedFromAnotherModel(model2);
  EXPECT_FALSE(awhp.addSpeed(speedFromAnotherModel));
  EXPECT_EQ(0, awhp.numExtensibleGroups());
  EXPECT_EQ(0, awhp.numberOfSpeeds());
  EXPECT_EQ(0, awhp.speeds().size());
}

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_clone) {

  // create a model to use
  Model model;

  HeatPumpAirToWaterHeating awhp(model);

  std::vector<HeatPumpAirToWaterHeatingSpeedData> speeds;
  for (unsigned i = 1; i <= HeatPumpAirToWaterHeating::maximum_number_of_speeds; ++i) {
    HeatPumpAirToWaterHeatingSpeedData speed(model);
    speed.setName("Speed " + std::to_string(i));
    speeds.push_back(speed);
    EXPECT_TRUE(awhp.addSpeed(speed));
  }

  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  EXPECT_EQ(1, model.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, model.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, model.getModelObjects<Curve>().size());

  auto awhpClone = awhp.clone(model).cast<HeatPumpAirToWaterHeating>();
  EXPECT_EQ(2, model.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(2, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, model.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, model.getModelObjects<Curve>().size());

  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  EXPECT_EQ(5, awhpClone.numberOfSpeeds());
  EXPECT_EQ(speeds, awhpClone.speeds());

  auto rmed = awhp.remove();

  EXPECT_EQ(2, rmed.size()) << getObjectNames(rmed);
  EXPECT_EQ(IddObjectType::OS_HeatPump_AirToWater_Heating, rmed[0].iddObject().type().value());
  EXPECT_EQ(IddObjectType::OS_ModelObjectList, rmed[1].iddObject().type().value());
  EXPECT_EQ(1, model.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, model.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, model.getModelObjects<Curve>().size());
  EXPECT_EQ(5, awhpClone.numberOfSpeeds());
  EXPECT_EQ(speeds, awhpClone.speeds());

  rmed = awhpClone.remove();

  const unsigned expectedRemoved = 1 /* HeatPumpAirToWaterHeating */ + 1 /* ModelObjectList */ + 5 * (1 /* SpeedData */ + 3 /* Curves */);
  EXPECT_EQ(expectedRemoved, rmed.size()) << getObjectNames(rmed);
  EXPECT_EQ(0, model.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(0, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(0, model.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(0, model.getModelObjects<Curve>().size());
}

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_addToNode) {
  // create a model to use
  Model m;

  HeatPumpAirToWaterHeating awhp(m);

  PlantLoop p(m);
  {
    Node n = p.demandInletNode();
    EXPECT_FALSE(awhp.addToNode(n));
  }
  EXPECT_FALSE(p.addDemandBranchForComponent(awhp));

  AirLoopHVAC a(m);
  {
    Node n = a.supplyInletNode();
    EXPECT_FALSE(awhp.addToNode(n));
  }
  {
    Node n = a.demandInletNode();
    EXPECT_FALSE(awhp.addToNode(n));
  }

  EXPECT_FALSE(awhp.plantLoop());
  EXPECT_FALSE(awhp.inletModelObject());
  EXPECT_FALSE(awhp.outletModelObject());

  // Plant Side connections
  EXPECT_EQ(5, p.demandComponents().size());  // o --- Splitter --- o --- Mixer --- o
  EXPECT_EQ(5, p.supplyComponents().size());  // o --- Splitter --- o --- Mixer --- o

  EXPECT_TRUE(p.addSupplyBranchForComponent(awhp));
  EXPECT_EQ(7, p.supplyComponents().size());  // o --- Splitter --- o --- awhp_hc --- Mixer --- o
  EXPECT_TRUE(awhp.plantLoop());
  EXPECT_TRUE(awhp.inletModelObject());
  EXPECT_TRUE(awhp.outletModelObject());

  auto awhpClone = awhp.clone(m).cast<HeatPumpAirToWaterHeating>();
  EXPECT_FALSE(awhpClone.plantLoop());
  EXPECT_FALSE(awhpClone.inletModelObject());
  EXPECT_FALSE(awhpClone.outletModelObject());

  EXPECT_EQ(2, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());

  awhp.removeFromLoop();
  EXPECT_FALSE(awhp.plantLoop());
  EXPECT_FALSE(awhp.inletModelObject());
  EXPECT_FALSE(awhp.outletModelObject());

  EXPECT_EQ(2, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(5, p.supplyComponents().size());  // o --- Splitter --- o --- Mixer --- o
  EXPECT_EQ(5, p.demandComponents().size());  // o --- Splitter --- o --- Mixer --- o

  auto supply_inlet_node = p.supplyInletNode();
  EXPECT_TRUE(awhpClone.addToNode(supply_inlet_node));

  EXPECT_EQ(7, p.supplyComponents().size());  // o --- awhpClone --- Splitter --- o --- Mixer --- o
  EXPECT_EQ(5, p.demandComponents().size());  // o --- Splitter --- o --- Mixer --- o
  EXPECT_TRUE(awhpClone.plantLoop());
  ASSERT_TRUE(awhpClone.inletModelObject());
  EXPECT_EQ(supply_inlet_node, awhpClone.inletModelObject().get());
  ASSERT_TRUE(awhpClone.outletModelObject());
  auto outlet_ = awhpClone.outletModelObject().get().optionalCast<Node>();
  ASSERT_TRUE(outlet_);
  ASSERT_TRUE(outlet_->outletModelObject());
  EXPECT_EQ(p.supplySplitter(), outlet_->outletModelObject().get());

  awhpClone.remove();
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(5, p.supplyComponents().size());  // o --- Splitter --- o --- Mixer --- o
  EXPECT_EQ(5, p.demandComponents().size());  // o --- Splitter --- o --- Mixer --- o
}

TEST_F(ModelFixture, HeatPumpAirToWaterHeating_containingHVACComponent) {

  Model m;

  HeatPumpAirToWater awhp(m);
  HeatPumpAirToWater awhp2(m);

  HeatPumpAirToWaterHeating awhp_hc(m);
  for (unsigned i = 1; i <= HeatPumpAirToWaterHeating::maximum_number_of_speeds; ++i) {
    HeatPumpAirToWaterHeatingSpeedData speed(m);
    speed.setName("Heating Speed " + std::to_string(i));
    EXPECT_TRUE(awhp_hc.addSpeed(speed));
  }
  EXPECT_FALSE(awhp_hc.containingHVACComponent());
  EXPECT_EQ(0, awhp_hc.heatPumpAirToWaters().size());

  EXPECT_TRUE(awhp.setHeatingOperationMode(awhp_hc));
  ASSERT_TRUE(awhp_hc.containingHVACComponent());
  EXPECT_EQ(awhp, awhp_hc.containingHVACComponent().get());
  EXPECT_EQ(1, awhp_hc.heatPumpAirToWaters().size());
  EXPECT_EQ(5, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  // Each speed has 3 curves
  EXPECT_EQ(5 * 3, m.getModelObjects<Curve>().size());

  EXPECT_TRUE(awhp2.setHeatingOperationMode(awhp_hc));
  ASSERT_TRUE(awhp_hc.containingHVACComponent());
  EXPECT_EQ(2, awhp_hc.heatPumpAirToWaters().size());

  EXPECT_TRUE(awhp.heatingOperationMode());
  EXPECT_TRUE(awhp2.heatingOperationMode());
  EXPECT_FALSE(awhp.coolingOperationMode());
  EXPECT_FALSE(awhp2.coolingOperationMode());

  // Is contained so not removable
  EXPECT_FALSE(awhp_hc.isRemovable());

  EXPECT_EQ(2, m.getConcreteModelObjects<HeatPumpAirToWater>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, m.getModelObjects<Curve>().size());

  EXPECT_EQ(0, awhp_hc.remove().size());
  EXPECT_EQ(2, m.getConcreteModelObjects<HeatPumpAirToWater>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, m.getModelObjects<Curve>().size());

  // Remove the first awhp
  auto rmed = awhp.remove();
  EXPECT_EQ(1, rmed.size()) << getObjectNames(rmed);
  EXPECT_EQ(1, awhp_hc.heatPumpAirToWaters().size());
  ASSERT_TRUE(awhp_hc.containingHVACComponent());
  EXPECT_EQ(awhp2, awhp_hc.containingHVACComponent().get());
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWater>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, m.getModelObjects<Curve>().size());

  // Still not removable
  EXPECT_FALSE(awhp_hc.isRemovable());
  rmed = awhp_hc.remove();
  EXPECT_EQ(0, rmed.size()) << getObjectNames(rmed);
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWater>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(1, m.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(5 * 3, m.getModelObjects<Curve>().size());

  // Remove the second awhp: this time it's not shared by any other object so it should remove everything
  rmed = awhp2.remove();
  const unsigned expectedRemoved =
    1 /* HeatPumpAirToWater */ + 1 /* HeatPumpAirToWaterHeating */ + 1 /* ModelObjectList */ + 5 * (1 /* SpeedData */ + 3 /* Curves */);
  EXPECT_EQ(expectedRemoved, rmed.size()) << getObjectNames(rmed);
  EXPECT_EQ(0, m.getConcreteModelObjects<HeatPumpAirToWater>().size());
  EXPECT_EQ(0, m.getConcreteModelObjects<HeatPumpAirToWaterHeating>().size());
  EXPECT_EQ(0, m.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(0, m.getConcreteModelObjects<HeatPumpAirToWaterHeatingSpeedData>().size());
  EXPECT_EQ(0, m.getModelObjects<Curve>().size());
}
