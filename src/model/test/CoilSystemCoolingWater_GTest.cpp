/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../CoilSystemCoolingWater.hpp"
#include "../CoilSystemCoolingWater_Impl.hpp"

// TODO: Check the following class names against object getters and setters.
#include "../Connection.hpp"
#include "../Connection_Impl.hpp"

#include "../Schedule.hpp"
#include "../Schedule_Impl.hpp"

#include "../CoolingCoilsWater.hpp"
#include "../CoolingCoilsWater_Impl.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, CoilSystemCoolingWater_GettersSetters) {
  Model m;
  // TODO: Check regular Ctor arguments
  CoilSystemCoolingWater coilSystemCoolingWater(m);
  // TODO: Or if a UniqueModelObject (and make sure _Impl is included)
  // CoilSystemCoolingWater coilSystemCoolingWater = m.getUniqueModelObject<CoilSystemCoolingWater>();

  coilSystemCoolingWater.setName("My CoilSystemCoolingWater");

  // Air Inlet Node Name: Required Object
  Connection airInletNode(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAirInletNode(airInletNode));
  EXPECT_EQ(airInletNode, coilSystemCoolingWater.airInletNode());

  // Air Outlet Node Name: Required Object
  Connection airOutletNode(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAirOutletNode(airOutletNode));
  EXPECT_EQ(airOutletNode, coilSystemCoolingWater.airOutletNode());

  // Availability Schedule Name: Required Object
  Schedule availabilitySchedule(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAvailabilitySchedule(availabilitySchedule));
  EXPECT_EQ(availabilitySchedule, coilSystemCoolingWater.availabilitySchedule());

  // Cooling Coil: Required Object
  CoolingCoilsWater coolingCoil(m);
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
  boost::optional<CoolingCoilsWater> companionCoilUsedForHeatRecovery(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery));
  ASSERT_TRUE(coilSystemCoolingWater.companionCoilUsedForHeatRecovery());
  EXPECT_EQ(companionCoilUsedForHeatRecovery, coilSystemCoolingWater.companionCoilUsedForHeatRecovery().get());
}
TEST_F(ModelFixture, CoilSystemCoolingWater_HeatCoolFuelTypes) {
  Model m;
  // TODO: Check regular Ctor arguments
  CoilSystemCoolingWater coilSystemCoolingWater(m);
  // TODO: Or if a UniqueModelObject (and make sure _Impl is included)
  // CoilSystemCoolingWater coilSystemCoolingWater = m.getUniqueModelObject<CoilSystemCoolingWater>();

  EXPECT_EQ(ComponentType(ComponentType::Both), coilSystemCoolingWater.componentType());
  testFuelTypeEquality({FuelType::Electricity}, coilSystemCoolingWater.coolingFuelTypes());
  testFuelTypeEquality({FuelType::Electricity, FuelType::Propane}, coilSystemCoolingWater.heatingFuelTypes());
  testAppGFuelTypeEquality({AppGFuelType::Fuel, AppGFuelType::HeatPump}, coilSystemCoolingWater.appGHeatingFuelTypes());
}
