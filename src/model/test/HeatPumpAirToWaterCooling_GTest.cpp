/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../HeatPumpAirToWaterCooling.hpp"
#include "../HeatPumpAirToWaterCooling_Impl.hpp"

#include "../HeatPumpAirToWaterCoolingSpeedData.hpp"
#include "../HeatPumpAirToWaterCoolingSpeedData_Impl.hpp"

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

#include "../ModelObjectList.hpp"
#include "../ModelObjectList_Impl.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, HeatPumpAirToWaterCooling_GettersSetters) {
  Model m;
  HeatPumpAirToWaterCooling awhp(m);

  awhp.setName("My HeatPumpAirToWaterCooling");

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
  HeatPumpAirToWaterCoolingSpeedData boosterModeOnSpeed(m);
  EXPECT_TRUE(awhp.setBoosterModeOnSpeed(boosterModeOnSpeed));
  ASSERT_TRUE(awhp.boosterModeOnSpeed());
  EXPECT_EQ(boosterModeOnSpeed, awhp.boosterModeOnSpeed().get());
  awhp.resetBoosterModeOnSpeed();
  EXPECT_FALSE(awhp.boosterModeOnSpeed());
}

TEST_F(ModelFixture, HeatPumpAirToWaterCooling_HeatCoolFuelTypes) {
  Model m;
  HeatPumpAirToWaterCooling awhp(m);

  EXPECT_EQ(ComponentType(ComponentType::Cooling), awhp.componentType());
  testFuelTypeEquality({FuelType::Electricity}, awhp.coolingFuelTypes());
  testFuelTypeEquality({}, awhp.heatingFuelTypes());
  testAppGFuelTypeEquality({}, awhp.appGHeatingFuelTypes());
}

TEST_F(ModelFixture, HeatPumpAirToWaterCooling_Speeds) {

  // create a model to use
  Model model;

  HeatPumpAirToWaterCooling awhp(model);

  std::vector<HeatPumpAirToWaterCoolingSpeedData> speeds;
  EXPECT_EQ(5, HeatPumpAirToWaterCooling::maximum_number_of_speeds);
  for (unsigned i = 1; i <= HeatPumpAirToWaterCooling::maximum_number_of_speeds; ++i) {
    HeatPumpAirToWaterCoolingSpeedData speed(model);
    speed.setName("Speed " + std::to_string(i));
    speeds.push_back(speed);
    EXPECT_TRUE(awhp.addSpeed(speed));
    EXPECT_EQ(i, awhp.numberOfSpeeds());
    EXPECT_EQ(speeds, awhp.speeds());
  }

  // Can't add more than 5 Speeds
  HeatPumpAirToWaterCoolingSpeedData anotherSpeed(model);
  EXPECT_FALSE(awhp.addSpeed(anotherSpeed));
  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  // Can't remove a speed that's not in there...
  EXPECT_FALSE(awhp.removeSpeed(anotherSpeed));
  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  {
    int speedIndex = 3;
    std::vector<HeatPumpAirToWaterCoolingSpeedData> thisSpeeds = awhp.speeds();
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
    std::vector<HeatPumpAirToWaterCoolingSpeedData> thisSpeeds = awhp.speeds();
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
    std::vector<HeatPumpAirToWaterCoolingSpeedData> thisSpeeds = awhp.speeds();
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
    HeatPumpAirToWaterCoolingSpeedData speed(model);
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
    std::vector<HeatPumpAirToWaterCoolingSpeedData> thisSpeeds = awhp.speeds();
    for (unsigned i = 0; i < 5; ++i) {
      EXPECT_EQ(speeds[i], thisSpeeds[i]);
    }
  }

  awhp.removeAllSpeeds();
  EXPECT_EQ(0, awhp.numberOfSpeeds());
  EXPECT_EQ(0, awhp.speeds().size());

  // Test that added a speed from another model will fail but not add a blank extensible group
  Model model2;
  HeatPumpAirToWaterCoolingSpeedData speedFromAnotherModel(model2);
  EXPECT_FALSE(awhp.addSpeed(speedFromAnotherModel));
  EXPECT_EQ(0, awhp.numExtensibleGroups());
  EXPECT_EQ(0, awhp.numberOfSpeeds());
  EXPECT_EQ(0, awhp.speeds().size());
}

TEST_F(ModelFixture, HeatPumpAirToWaterCooling_clone) {

  // create a model to use
  Model model;

  HeatPumpAirToWaterCooling awhp(model);

  std::vector<HeatPumpAirToWaterCoolingSpeedData> speeds;
  for (unsigned i = 1; i <= HeatPumpAirToWaterCooling::maximum_number_of_speeds; ++i) {
    HeatPumpAirToWaterCoolingSpeedData speed(model);
    speed.setName("Speed " + std::to_string(i));
    speeds.push_back(speed);
    EXPECT_TRUE(awhp.addSpeed(speed));
  }

  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  EXPECT_EQ(1, model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().size());
  EXPECT_EQ(1, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, model.getConcreteModelObjects<HeatPumpAirToWaterCoolingSpeedData>().size());

  auto awhpClone = awhp.clone(model).cast<HeatPumpAirToWaterCooling>();
  EXPECT_EQ(2, model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().size());
  EXPECT_EQ(2, model.getConcreteModelObjects<ModelObjectList>().size());
  EXPECT_EQ(5, model.getConcreteModelObjects<HeatPumpAirToWaterCoolingSpeedData>().size());

  EXPECT_EQ(5, awhp.numberOfSpeeds());
  EXPECT_EQ(speeds, awhp.speeds());

  EXPECT_EQ(5, awhpClone.numberOfSpeeds());
  EXPECT_EQ(speeds, awhpClone.speeds());

  // This shouldn't work, it's going to put the CoilCoolingDXCurveFitPerformance in a broken state
  auto rmed = awhp.remove();
  EXPECT_EQ(0, rmed.size());
  HeatPumpAirToWaterCoolingSpeedData boosterModeOnSpeed(model);
  EXPECT_TRUE(awhp.setBoosterModeOnSpeed(boosterModeOnSpeed));
  ASSERT_TRUE(awhp.boosterModeOnSpeed());
  EXPECT_EQ(3, model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().size());
  EXPECT_EQ(10, model.getConcreteModelObjects<HeatPumpAirToWaterCoolingSpeedData>().size());

  rmed = awhp.remove();
  EXPECT_EQ(1, rmed.size());
  EXPECT_EQ(2, model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().size());
  EXPECT_EQ(10, model.getConcreteModelObjects<HeatPumpAirToWaterCoolingSpeedData>().size());
  EXPECT_EQ(10, awhpClone.numberOfSpeeds());
  EXPECT_EQ(speeds, awhpClone.speeds());

  rmed = awhpClone.remove();
  EXPECT_EQ(11, rmed.size());
  EXPECT_EQ(1, model.getConcreteModelObjects<HeatPumpAirToWaterCooling>().size());
  EXPECT_EQ(0, model.getConcreteModelObjects<HeatPumpAirToWaterCoolingSpeedData>().size());
}
