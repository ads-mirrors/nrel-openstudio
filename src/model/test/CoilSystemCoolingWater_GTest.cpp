/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../CoilSystemCoolingWater.hpp"
#include "../CoilSystemCoolingWater_Impl.hpp"
#include "../Schedule.hpp"
#include "../Schedule_Impl.hpp"
#include "../CoilCoolingWater.hpp"
#include "../CoilCoolingWater_Impl.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, CoilSystemCoolingWater_GettersSetters) {
  Model m;

  CoilSystemCoolingWater coilSystemCoolingWater(m);

  coilSystemCoolingWater.setName("My CoilSystemCoolingWater");

  // Availability Schedule Name: Required Object
  Schedule availabilitySchedule = m.alwaysOnDiscreteSchedule();
  EXPECT_TRUE(coilSystemCoolingWater.setAvailabilitySchedule(availabilitySchedule));
  EXPECT_EQ(availabilitySchedule, coilSystemCoolingWater.availabilitySchedule());

  // Cooling Coil: Required Object
  CoilCoolingWater coolingCoil(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCoolingCoil(coolingCoil));
  EXPECT_EQ(coolingCoil, coilSystemCoolingWater.coolingCoil());

  // Dehumidification Control Type: Required String
  EXPECT_TRUE(coilSystemCoolingWater.setDehumidificationControlType("None"));
  EXPECT_EQ("None", coilSystemCoolingWater.dehumidificationControlType());
  // Bad Value
  EXPECT_FALSE(coilSystemCoolingWater.setDehumidificationControlType("BADENUM"));
  EXPECT_EQ("None", coilSystemCoolingWater.dehumidificationControlType());

  // Run on Sensible Load: Required Boolean
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(true));
  EXPECT_TRUE(coilSystemCoolingWater.runonSensibleLoad());
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(false));
  EXPECT_FALSE(coilSystemCoolingWater.runonSensibleLoad());

  // Run on Latent Load: Required Boolean
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(true));
  EXPECT_TRUE(coilSystemCoolingWater.runonLatentLoad());
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(false));
  EXPECT_FALSE(coilSystemCoolingWater.runonLatentLoad());

  // Minimum Air To Water Temperature Offset: Required Double
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(1.0));
  EXPECT_EQ(1.0, coilSystemCoolingWater.minimumAirToWaterTemperatureOffset());
  // Bad Value
  EXPECT_FALSE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(-10.0));
  EXPECT_EQ(1.0, coilSystemCoolingWater.minimumAirToWaterTemperatureOffset());

  // Economizer Lockout: Required Boolean
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(true));
  EXPECT_TRUE(coilSystemCoolingWater.economizerLockout());
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(false));
  EXPECT_FALSE(coilSystemCoolingWater.economizerLockout());

  // Minimum Water Loop Temperature For Heat Recovery: Required Double
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumWaterLoopTemperatureForHeatRecovery(1.2));
  EXPECT_EQ(1.2, coilSystemCoolingWater.minimumWaterLoopTemperatureForHeatRecovery());

  // Companion Coil Used For Heat Recovery: Optional Object
  CoilCoolingWater companionCoilUsedForHeatRecovery(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery));
  ASSERT_TRUE(coilSystemCoolingWater.companionCoilUsedForHeatRecovery());
  EXPECT_EQ(companionCoilUsedForHeatRecovery, coilSystemCoolingWater.companionCoilUsedForHeatRecovery().get());
}

TEST_F(ModelFixture, CoilSystemCoolingWater_HeatCoolFuelTypes) {
  Model m;

  CoilSystemCoolingWater coilSystemCoolingWater(m);

  EXPECT_EQ(ComponentType(ComponentType::Cooling), coilSystemCoolingWater.componentType());
  testFuelTypeEquality({}, coilSystemCoolingWater.coolingFuelTypes());
  testFuelTypeEquality({}, coilSystemCoolingWater.heatingFuelTypes());
  testAppGFuelTypeEquality({}, coilSystemCoolingWater.appGHeatingFuelTypes());
}

TEST_F(ModelFixture, CoilSystemCoolingWater_clone) {}

TEST_F(ModelFixture, CoilSystemCoolingWater_remove) {}

TEST_F(ModelFixture, CoilSystemCoolingWater_addToNode) {}
