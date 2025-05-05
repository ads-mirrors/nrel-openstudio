/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../CoilSystemCoolingWater.hpp"
#include "../CoilSystemCoolingWater_Impl.hpp"

#include "../CoilCoolingWater.hpp"
#include "../CoilCoolingWater_Impl.hpp"
#include "../CoilSystemCoolingWaterHeatExchangerAssisted.hpp"
#include "../CoilSystemCoolingWaterHeatExchangerAssisted_Impl.hpp"
#include "../AirLoopHVAC.hpp"
#include "../ChillerElectricEIR.hpp"
#include "../Node.hpp"
#include "../PlantLoop.hpp"
#include "../Schedule.hpp"
#include "../ScheduleConstant.hpp"
#include "../ScheduleTypeLimits.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, CoilSystemCoolingWater_GettersSetters) {
  Model m;

  CoilSystemCoolingWater coilSystemCoolingWater(m);

  coilSystemCoolingWater.setName("My CoilSystemCoolingWater");

  // Availability Schedule Name: Required Object
  // Ctor default
  Schedule availabilitySchedule = m.alwaysOnDiscreteSchedule();
  EXPECT_EQ(availabilitySchedule, coilSystemCoolingWater.availabilitySchedule());
  // set
  ScheduleConstant availSch(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAvailabilitySchedule(availSch));
  EXPECT_EQ(availSch, coilSystemCoolingWater.availabilitySchedule());
  // The Schedule Type Registry should have done it's thing and assigned a scheduleTypeLimits
  ASSERT_TRUE(availSch.scheduleTypeLimits());
  {
    auto sch_type_lim = availSch.scheduleTypeLimits().get();
    EXPECT_EQ("OnOff", sch_type_lim.nameString());
    ASSERT_TRUE(sch_type_lim.lowerLimitValue());
    EXPECT_EQ(0.0, sch_type_lim.lowerLimitValue().get());
    ASSERT_TRUE(sch_type_lim.upperLimitValue());
    EXPECT_EQ(1.0, sch_type_lim.upperLimitValue().get());
    ASSERT_TRUE(sch_type_lim.numericType());
    EXPECT_EQ("Discrete", sch_type_lim.numericType().get());
    EXPECT_EQ("Availability", sch_type_lim.unitType());
  }

  // Cooling Coil: Required Object
  // Ctor instantiates a CoilCoolingWater
  EXPECT_TRUE(coilSystemCoolingWater.coolingCoil().optionalCast<CoilCoolingWater>());
  CoilCoolingWater coolingCoil(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCoolingCoil(coolingCoil));
  EXPECT_EQ(coolingCoil, coilSystemCoolingWater.coolingCoil());

  CoilSystemCoolingWaterHeatExchangerAssisted coilSystemCoolingWaterHeatExchangerAssisted(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCoolingCoil(coilSystemCoolingWaterHeatExchangerAssisted));
  EXPECT_EQ(coilSystemCoolingWaterHeatExchangerAssisted, coilSystemCoolingWater.coolingCoil());

  // Dehumidification Control Type: Required String
  // Ctor default
  EXPECT_EQ("None", coilSystemCoolingWater.dehumidificationControlType());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setDehumidificationControlType("CoolReheat"));
  EXPECT_EQ("CoolReheat", coilSystemCoolingWater.dehumidificationControlType());
  // Bad Value
  EXPECT_FALSE(coilSystemCoolingWater.setDehumidificationControlType("BADENUM"));
  EXPECT_EQ("CoolReheat", coilSystemCoolingWater.dehumidificationControlType());

  // Run on Sensible Load: Required Boolean
  // Ctor default
  EXPECT_TRUE(coilSystemCoolingWater.runonSensibleLoad());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(false));
  EXPECT_FALSE(coilSystemCoolingWater.runonSensibleLoad());
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(true));
  EXPECT_TRUE(coilSystemCoolingWater.runonSensibleLoad());

  // Run on Latent Load: Required Boolean
  // Ctor default
  EXPECT_FALSE(coilSystemCoolingWater.runonLatentLoad());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(true));
  EXPECT_TRUE(coilSystemCoolingWater.runonLatentLoad());
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(false));
  EXPECT_FALSE(coilSystemCoolingWater.runonLatentLoad());

  // Minimum Air To Water Temperature Offset: Required Double
  // Ctor default
  EXPECT_EQ(0.0, coilSystemCoolingWater.minimumAirToWaterTemperatureOffset());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(1.0));
  EXPECT_EQ(1.0, coilSystemCoolingWater.minimumAirToWaterTemperatureOffset());
  // Bad Value
  EXPECT_FALSE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(-10.0));
  EXPECT_EQ(1.0, coilSystemCoolingWater.minimumAirToWaterTemperatureOffset());

  // Economizer Lockout: Required Boolean
  // Ctor default
  EXPECT_TRUE(coilSystemCoolingWater.economizerLockout());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(false));
  EXPECT_FALSE(coilSystemCoolingWater.economizerLockout());
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(true));
  EXPECT_TRUE(coilSystemCoolingWater.economizerLockout());

  // Minimum Water Loop Temperature For Heat Recovery: Required Double
  // Ctor default
  EXPECT_EQ(0.0, coilSystemCoolingWater.minimumWaterLoopTemperatureForHeatRecovery());
  // set
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumWaterLoopTemperatureForHeatRecovery(1.2));
  EXPECT_EQ(1.2, coilSystemCoolingWater.minimumWaterLoopTemperatureForHeatRecovery());

  // Companion Coil Used For Heat Recovery: Optional Object
  // The only type allowed is CoilCoolingWater
  CoilCoolingWater companionCoilUsedForHeatRecovery(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery));
  ASSERT_TRUE(coilSystemCoolingWater.companionCoilUsedForHeatRecovery());
  EXPECT_EQ(companionCoilUsedForHeatRecovery, coilSystemCoolingWater.companionCoilUsedForHeatRecovery().get());

  CoilSystemCoolingWaterHeatExchangerAssisted wrongCompanionCoil(m);
  EXPECT_FALSE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(wrongCompanionCoil));
  ASSERT_TRUE(coilSystemCoolingWater.companionCoilUsedForHeatRecovery());
  EXPECT_EQ(companionCoilUsedForHeatRecovery, coilSystemCoolingWater.companionCoilUsedForHeatRecovery().get());
}

TEST_F(ModelFixture, CoilSystemCoolingWater_HeatCoolFuelTypes) {
  Model m;

  CoilSystemCoolingWater coilSystem(m);
  auto cc = coilSystem.coolingCoil().cast<CoilCoolingWater>();

  EXPECT_EQ(ComponentType(ComponentType::Cooling), coilSystem.componentType());
  testFuelTypeEquality({}, coilSystem.coolingFuelTypes());
  testFuelTypeEquality({}, coilSystem.heatingFuelTypes());
  testAppGFuelTypeEquality({}, coilSystem.appGHeatingFuelTypes());

  PlantLoop p(m);
  ChillerElectricEIR ch(m);
  EXPECT_TRUE(p.addSupplyBranchForComponent(ch));
  EXPECT_TRUE(p.addDemandBranchForComponent(cc));
  EXPECT_EQ(ComponentType(ComponentType::Cooling), coilSystem.componentType());
  testFuelTypeEquality({FuelType::Electricity}, coilSystem.coolingFuelTypes());
  testFuelTypeEquality({}, coilSystem.heatingFuelTypes());
  testAppGFuelTypeEquality({}, coilSystem.appGHeatingFuelTypes());
}

TEST_F(ModelFixture, CoilSystemCoolingWater_clone) {}

TEST_F(ModelFixture, CoilSystemCoolingWater_remove) {}

TEST_F(ModelFixture, CoilSystemCoolingWater_addToNode) {
  // Water Size Economizer Model
  {
    Model m;
    CoilSystemCoolingWater coilSystem(m);

    AirLoopHVAC a(m);
    Node n = a.supplyOutletNode();

    auto cc = coilSystem.coolingCoil().cast<CoilCoolingWater>();

    EXPECT_EQ(2u, a.supplyComponents().size());

    EXPECT_TRUE(cc.addToNode(n));
    EXPECT_EQ(3u, a.supplyComponents().size());

    EXPECT_TRUE(coilSystem.addToNode(n));
    EXPECT_EQ(5u, a.supplyComponents().size());

    {
      auto containingHVACComponent = cc.containingHVACComponent();
      ASSERT_TRUE(containingHVACComponent);
      EXPECT_EQ(containingHVACComponent->handle(), coilSystem.handle());
    }

    // BUT, we need to be able to connect the water side of the Coil...
    PlantLoop p(m);
    EXPECT_TRUE(p.addDemandBranchForComponent(cc));
  }

  // Wrap Around Water Coil Heat Recovery Mode
  {
    Model m;
    CoilSystemCoolingWater coilSystem(m);
    CoilCoolingWater companionCoil(m);
    coilSystem.setCompanionCoilUsedForHeatRecovery(companionCoil);

    AirLoopHVAC a(m);
    Node n = a.supplyOutletNode();

    auto cc = coilSystem.coolingCoil().cast<CoilCoolingWater>();
    auto hr = coilSystem.companionCoilUsedForHeatRecovery().get().cast<CoilCoolingWater>();

    EXPECT_EQ(2u, a.supplyComponents().size());

    EXPECT_TRUE(cc.addToNode(n));
    EXPECT_EQ(3u, a.supplyComponents().size());

    EXPECT_TRUE(hr.addToNode(n));
    EXPECT_EQ(5u, a.supplyComponents().size());

    EXPECT_TRUE(coilSystem.addToNode(n));
    EXPECT_EQ(7u, a.supplyComponents().size());

    {
      auto containingHVACComponent = cc.containingHVACComponent();
      ASSERT_TRUE(containingHVACComponent);
      EXPECT_EQ(containingHVACComponent->handle(), coilSystem.handle());
    }

    {
      auto containingHVACComponent = hr.containingHVACComponent();
      ASSERT_TRUE(containingHVACComponent);
      EXPECT_EQ(containingHVACComponent->handle(), coilSystem.handle());
    }

    // BUT, we need to be able to connect the water side of the Coil...
    PlantLoop p(m);
    EXPECT_TRUE(p.addDemandBranchForComponent(cc));
  }
}
