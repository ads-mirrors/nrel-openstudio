/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "EnergyPlusFixture.hpp"

#include "../ForwardTranslator.hpp"
#include "../ReverseTranslator.hpp"

#include "../../model/CoilSystemCoolingWater.hpp"
#include "../../model/CoilSystemCoolingWater_Impl.hpp"
// TODO: Check the following class names against object getters and setters.
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/CoolingCoilsWater.hpp"
#include "../../model/CoolingCoilsWater_Impl.hpp"
#include "../../model/CoolingCoilsWater.hpp"
#include "../../model/CoolingCoilsWater_Impl.hpp"

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
  // TODO: Check regular Ctor arguments
  CoilSystemCoolingWater coilSystemCoolingWater(m);
  // TODO: Or if a UniqueModelObject (and make sure _Impl is included)
  // CoilSystemCoolingWater coilSystemCoolingWater = m.getUniqueModelObject<CoilSystemCoolingWater>();

  coilSystemCoolingWater.setName("My CoilSystemCoolingWater");
  Node airInletNodeName(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAirInletNodeName(airInletNodeName));
  Node airOutletNodeName(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAirOutletNodeName(airOutletNodeName));
  boost::optional<Schedule> availabilitySchedule(m);
  EXPECT_TRUE(coilSystemCoolingWater.setAvailabilitySchedule(availabilitySchedule));
  EXPECT_TRUE(coilSystemCoolingWater.setCoolingCoilObjectType("Coil:Cooling:Water"));
  CoolingCoilsWater coolingCoil(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCoolingCoil(coolingCoil));
  EXPECT_TRUE(coilSystemCoolingWater.setDehumidificationControlType("None"));
  EXPECT_TRUE(coilSystemCoolingWater.setRunonSensibleLoad(false));  // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setRunonLatentLoad(true));     // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumAirToWaterTemperatureOffset(1.0));
  EXPECT_TRUE(coilSystemCoolingWater.setEconomizerLockout(false));  // Opposite from IDD default
  EXPECT_TRUE(coilSystemCoolingWater.setMinimumWaterLoopTemperatureForHeatRecovery(1.2));
  boost::optional<CoolingCoilsWater> companionCoilUsedForHeatRecovery(m);
  EXPECT_TRUE(coilSystemCoolingWater.setCompanionCoilUsedForHeatRecovery(companionCoilUsedForHeatRecovery));

  // TODO: you're responsible for creating all other objects needed so this object actually gets ForwardTranslated

  const Workspace w = ft.translateModel(m);
  const auto idfObjs = w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water);
  ASSERT_EQ(1u, idfObjs.size());

  const auto& idfObject = idfObjs.front();
  EXPECT_EQ(airInletNodeName.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get());
  EXPECT_EQ(airOutletNodeName.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get());
  EXPECT_EQ(availabilitySchedule.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName).get());
  EXPECT_EQ("Coil:Cooling:Water", idfObject.getString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType).get());
  EXPECT_EQ(coolingCoil.nameString(), idfObject.getString(CoilSystem_Cooling_WaterFields::CoolingCoilName).get());
  EXPECT_EQ("None", idfObject.getString(CoilSystem_Cooling_WaterFields::DehumidificationControlType).get());
  EXPECT_EQ("No", idfObject.getString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad).get());
  EXPECT_EQ("Yes", idfObject.getString(CoilSystem_Cooling_WaterFields::RunonLatentLoad).get());
  EXPECT_EQ(1.0, idfObject.getDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset).get());
  EXPECT_EQ("No", idfObject.getString(CoilSystem_Cooling_WaterFields::EconomizerLockout).get());
  EXPECT_EQ(1.2, idfObject.getDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery).get());
  EXPECT_EQ(companionCoilUsedForHeatRecovery.nameString(),
            idfObject.getString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery).get());
}
