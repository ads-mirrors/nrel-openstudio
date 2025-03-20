/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "EnergyPlusFixture.hpp"

#include "../ForwardTranslator.hpp"

#include "../../model/CoilSystemCoolingWater.hpp"
#include "../../model/CoilSystemCoolingWater_Impl.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/CoilCoolingWater.hpp"
#include "../../model/CoilCoolingWater_Impl.hpp"

#include "../../model/AirLoopHVAC.hpp"
#include "../../model/AirLoopHVACUnitarySystem.hpp"
#include "../../model/Node.hpp"

#include "../../utilities/idf/Workspace.hpp"
#include "../../utilities/idf/IdfObject.hpp"
#include "../../utilities/idf/WorkspaceObject.hpp"

// E+ FieldEnums
#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/CoilSystem_Cooling_Water_FieldEnums.hxx>

using namespace openstudio::energyplus;
using namespace openstudio::model;
using namespace openstudio;

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilSystemCoolingWater) {

  ForwardTranslator ft;

  Model m;

  CoilSystemCoolingWater coilSystemCoolingWater(m);

  coilSystemCoolingWater.setName("My CoilSystemCoolingWater");
  Schedule availabilitySchedule = m.alwaysOnDiscreteSchedule();
  EXPECT_TRUE(coilSystemCoolingWater.setAvailabilitySchedule(availabilitySchedule));
  EXPECT_TRUE(coilSystemCoolingWater.setDehumidificationControlType("CoolReheat"));
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(false));  // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(true));     // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(1.0));
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(false));  // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumWaterLoopTemperatureForHeatRecovery(1.2));
  CoilCoolingWater companionCoilUsedForHeatRecovery(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery));

  // put it inside a Unitary, and put Unitary on an AirLoopHVAC so it gets translated
  AirLoopHVACUnitarySystem unitary(m);
  unitary.setCoolingCoil(coilSystemCoolingWater);
  AirLoopHVAC airLoop(m);
  Node supplyOutletNode = airLoop.supplyOutletNode();
  unitary.addToNode(supplyOutletNode);
  ASSERT_TRUE(unitary.inletNode());
  EXPECT_TRUE(unitary.inletNode()->setName("Coil Air Inlet Node"));
  ASSERT_TRUE(unitary.outletNode());
  EXPECT_TRUE(unitary.outletNode()->setName("Coil Air Outlet Node"));

  // They must be connected to a PlantLoop too
  PlantLoop chw_p(m);
  chw_p.addDemandBranchForComponent(coilSystemCoolingWater.coolingCoil());

  const Workspace w = ft.translateModel(m);
  const auto idfObjs = w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water);
  ASSERT_EQ(1u, idfObjs.size());

  const auto& idfObject = idfObjs.front();
  EXPECT_EQ(coil.inletModelObject().get().nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get());
  EXPECT_EQ(coil.outletModelObject().get().nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get());
  EXPECT_EQ(availabilitySchedule.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName).get());
  EXPECT_EQ("Coil:Cooling:Water", idfObject.getString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType).get());
  EXPECT_EQ(coolingCoil.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::CoolingCoilName).get());
  EXPECT_EQ("CoolReheat", idfObject.getString(CoilSystem_Cooling_WaterFields::DehumidificationControlType).get());
  EXPECT_EQ("No", idfObject.getString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad).get());
  EXPECT_EQ("Yes", idfObject.getString(CoilSystem_Cooling_WaterFields::RunonLatentLoad).get());
  EXPECT_EQ(1.0, idfObject.getDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset).get());
  EXPECT_EQ("No", idfObject.getString(CoilSystem_Cooling_WaterFields::EconomizerLockout).get());
  EXPECT_EQ(1.2, idfObject.getDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery).get());
  EXPECT_EQ(companionCoilUsedForHeatRecovery.nameString(),
            idfObject.getString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery).get());
}
