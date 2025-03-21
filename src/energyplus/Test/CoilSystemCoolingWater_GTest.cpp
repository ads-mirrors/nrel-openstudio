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
#include "../../utilities/idf/IdfExtensibleGroup.hpp"
#include "../../utilities/idf/WorkspaceExtensibleGroup.hpp"

// E+ FieldEnums
#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/CoilSystem_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/Coil_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/AirLoopHVAC_FieldEnums.hxx>
#include <utilities/idd/BranchList_FieldEnums.hxx>
#include <utilities/idd/Branch_FieldEnums.hxx>
#include <utilities/idd/AirLoopHVAC_UnitarySystem_FieldEnums.hxx>
#include <utilities/idd/Controller_WaterCoil_FieldEnums.hxx>

using namespace openstudio::energyplus;
using namespace openstudio::model;
using namespace openstudio;

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilSystemCoolingWater_Unitary) {

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

  const auto idfUnitarys = w.getObjectsByType(IddObjectType::AirLoopHVAC_UnitarySystem);
  ASSERT_EQ(1u, idfUnitarys.size());
  const auto& idfUnitary = idfUnitarys.front();

  const auto idfCoilSystems = w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water);
  ASSERT_EQ(1u, idfCoilSystems.size());  // FIXME: should unitary here be allowed? if so, we need to update those FT files?
  const auto& idfCoilSystem = idfCoilSystems.front();

  WorkspaceObjectVector idfCoils(w.getObjectsByType(IddObjectType::Coil_Cooling_Water));
  ASSERT_EQ(1u, idfCoils.size());
  WorkspaceObject idfCoil(idfCoils[0]);

  // Check that the Unitary ends up with the CoilSystemCoolingWater
  EXPECT_EQ("CoilSystem:Cooling:Water", idfUnitary.getString(AirLoopHVAC_UnitarySystemFields::CoolingCoilObjectType).get());
  EXPECT_EQ(idfCoilSystem.nameString(), idfUnitary.getString(AirLoopHVAC_UnitarySystemFields::CoolingCoilName).get());

  EXPECT_EQ(coilSystemCoolingWater.inletModelObject().get().nameString(),
            idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get());
  EXPECT_EQ(coilSystemCoolingWater.outletModelObject().get().nameString(),
            idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get());
  EXPECT_EQ(availabilitySchedule.nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName).get());
  EXPECT_EQ("Coil:Cooling:Water", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType).get());
  EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilName).get());
  EXPECT_EQ("CoolReheat", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::DehumidificationControlType).get());
  EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad).get());
  EXPECT_EQ("Yes", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonLatentLoad).get());
  EXPECT_EQ(1.0, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset).get());
  EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::EconomizerLockout).get());
  EXPECT_EQ(1.2, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery).get());
  EXPECT_EQ(companionCoilUsedForHeatRecovery.nameString(),
            idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery).get());

  EXPECT_EQ(0, w.getObjectsByType(IddObjectType::Controller_WaterCoil).size());

  EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoil.getString(Coil_Cooling_WaterFields::Name).get());
  EXPECT_EQ("Always On Discrete", idfCoil.getString(Coil_Cooling_WaterFields::AvailabilityScheduleName).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignWaterFlowRate).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignAirFlowRate).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletWaterTemperature).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirTemperature).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirHumidityRatio).get());
  EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignOutletAirHumidityRatio).get());
  EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterInletModelObject().get().nameString(),
            idfCoil.getString(Coil_Cooling_WaterFields::WaterInletNodeName).get());
  EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterOutletModelObject().get().nameString(),
            idfCoil.getString(Coil_Cooling_WaterFields::WaterOutletNodeName).get());
  EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get(),
            idfCoil.getString(Coil_Cooling_WaterFields::AirInletNodeName).get());
  EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get(),
            idfCoil.getString(Coil_Cooling_WaterFields::AirOutletNodeName).get());
  EXPECT_EQ("SimpleAnalysis", idfCoil.getString(Coil_Cooling_WaterFields::TypeofAnalysis).get());
  EXPECT_EQ("CrossFlow", idfCoil.getString(Coil_Cooling_WaterFields::HeatExchangerConfiguration).get());
  EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::CondensateCollectionWaterStorageTankName));
  EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::DesignWaterTemperatureDifference));
}

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilSystemCoolingWater_AirLoopHVAC) {
  // without companion
  {
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

    // put it inside a Unitary, and put Unitary on an AirLoopHVAC so it gets translated
    AirLoopHVAC airLoop(m);
    Node supplyOutletNode = airLoop.supplyOutletNode();
    EXPECT_TRUE(coilSystemCoolingWater.addToNode(supplyOutletNode));

    // They must be connected to a PlantLoop too
    PlantLoop chw_p(m);
    chw_p.addDemandBranchForComponent(coilSystemCoolingWater.coolingCoil());

    const Workspace w = ft.translateModel(m);

    const auto idfCoilSystems = w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water);
    ASSERT_EQ(1u, idfCoilSystems.size());
    const auto& idfCoilSystem = idfCoilSystems.front();

    WorkspaceObjectVector idfCoils(w.getObjectsByType(IddObjectType::Coil_Cooling_Water));
    ASSERT_EQ(1u, idfCoils.size());
    WorkspaceObject idfCoil(idfCoils[0]);

    // Go from AirLoopHVAC to BranchList to Branch
    WorkspaceObjectVector idf_airloops = w.getObjectsByType(IddObjectType::AirLoopHVAC);
    ASSERT_EQ(1u, idf_airloops.size());

    WorkspaceObject& idf_airLoopHVAC = idf_airloops.front();
    WorkspaceObject idf_brlist = idf_airLoopHVAC.getTarget(AirLoopHVACFields::BranchListName).get();

    // Should have one branch only
    ASSERT_EQ(1u, idf_brlist.extensibleGroups().size());
    auto w_eg = idf_brlist.extensibleGroups()[0].cast<WorkspaceExtensibleGroup>();
    WorkspaceObject idf_branch = w_eg.getTarget(BranchListExtensibleFields::BranchName).get();

    // There should be only one equipment on the branch
    ASSERT_EQ(1u, idf_branch.extensibleGroups().size());
    auto w_eg2 = idf_branch.extensibleGroups()[0].cast<WorkspaceExtensibleGroup>();

    EXPECT_EQ(w_eg2.getString(BranchExtensibleFields::ComponentInletNodeName).get(), coilSystemCoolingWater.inletModelObject().get().nameString());
    EXPECT_EQ(w_eg2.getString(BranchExtensibleFields::ComponentOutletNodeName).get(), coilSystemCoolingWater.outletModelObject().get().nameString());
    EXPECT_EQ("CoilSystem:Cooling:Water", w_eg2.getString(BranchExtensibleFields::ComponentObjectType).get());

    EXPECT_EQ(coilSystemCoolingWater.inletModelObject().get().nameString(),
              idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get());
    EXPECT_EQ(coilSystemCoolingWater.outletModelObject().get().nameString(),
              idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get());
    EXPECT_EQ(availabilitySchedule.nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName).get());
    EXPECT_EQ("Coil:Cooling:Water", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilName).get());
    EXPECT_EQ("CoolReheat", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::DehumidificationControlType).get());
    EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad).get());
    EXPECT_EQ("Yes", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonLatentLoad).get());
    EXPECT_EQ(1.0, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset).get());
    EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::EconomizerLockout).get());
    EXPECT_EQ(1.2, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery).get());
    EXPECT_TRUE(idfCoilSystem.isEmpty(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery));

    EXPECT_EQ(0, w.getObjectsByType(IddObjectType::Controller_WaterCoil)
                   .size());  // FIXME: what's creating this if i have it commented out in the addToNode method?

    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoil.getString(Coil_Cooling_WaterFields::Name).get());
    EXPECT_EQ("Always On Discrete", idfCoil.getString(Coil_Cooling_WaterFields::AvailabilityScheduleName).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignWaterFlowRate).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignAirFlowRate).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletWaterTemperature).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirTemperature).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirHumidityRatio).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignOutletAirHumidityRatio).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterInletModelObject().get().nameString(),
              idfCoil.getString(Coil_Cooling_WaterFields::WaterInletNodeName).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterOutletModelObject().get().nameString(),
              idfCoil.getString(Coil_Cooling_WaterFields::WaterOutletNodeName).get());
    EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get(),
              idfCoil.getString(Coil_Cooling_WaterFields::AirInletNodeName).get());
    EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get(),
              idfCoil.getString(Coil_Cooling_WaterFields::AirOutletNodeName).get());
    EXPECT_EQ("SimpleAnalysis", idfCoil.getString(Coil_Cooling_WaterFields::TypeofAnalysis).get());
    EXPECT_EQ("CrossFlow", idfCoil.getString(Coil_Cooling_WaterFields::HeatExchangerConfiguration).get());
    EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::CondensateCollectionWaterStorageTankName));
    EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::DesignWaterTemperatureDifference));
  }

  // with companion
  {
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
    AirLoopHVAC airLoop(m);
    Node supplyOutletNode = airLoop.supplyOutletNode();
    EXPECT_TRUE(coilSystemCoolingWater.addToNode(supplyOutletNode));

    // They must be connected to a PlantLoop too
    PlantLoop chw_p(m);
    chw_p.addDemandBranchForComponent(coilSystemCoolingWater.coolingCoil());

    const Workspace w = ft.translateModel(m);

    const auto idfCoilSystems = w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water);
    ASSERT_EQ(1u, idfCoilSystems.size());
    const auto& idfCoilSystem = idfCoilSystems.front();

    WorkspaceObjectVector idfCoils(w.getObjectsByType(IddObjectType::Coil_Cooling_Water));
    ASSERT_EQ(2u, idfCoils.size());
    WorkspaceObject idfCoil(idfCoils[0]);  // FIXME: get these some other way

    // Go from AirLoopHVAC to BranchList to Branch
    WorkspaceObjectVector idf_airloops = w.getObjectsByType(IddObjectType::AirLoopHVAC);
    ASSERT_EQ(1u, idf_airloops.size());

    WorkspaceObject& idf_airLoopHVAC = idf_airloops.front();
    WorkspaceObject idf_brlist = idf_airLoopHVAC.getTarget(AirLoopHVACFields::BranchListName).get();

    // Should have one branch only
    ASSERT_EQ(1u, idf_brlist.extensibleGroups().size());
    auto w_eg = idf_brlist.extensibleGroups()[0].cast<WorkspaceExtensibleGroup>();
    WorkspaceObject idf_branch = w_eg.getTarget(BranchListExtensibleFields::BranchName).get();

    // There should be only one equipment on the branch
    ASSERT_EQ(1u, idf_branch.extensibleGroups().size());
    auto w_eg2 = idf_branch.extensibleGroups()[0].cast<WorkspaceExtensibleGroup>();

    EXPECT_EQ(w_eg2.getString(BranchExtensibleFields::ComponentInletNodeName).get(), coilSystemCoolingWater.inletModelObject().get().nameString());
    EXPECT_EQ(w_eg2.getString(BranchExtensibleFields::ComponentOutletNodeName).get(), coilSystemCoolingWater.outletModelObject().get().nameString());
    EXPECT_EQ("CoilSystem:Cooling:Water", w_eg2.getString(BranchExtensibleFields::ComponentObjectType).get());

    EXPECT_EQ(coilSystemCoolingWater.inletModelObject().get().nameString(),
              idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get());
    EXPECT_EQ(coilSystemCoolingWater.outletModelObject().get().nameString(),
              idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get());
    EXPECT_EQ(availabilitySchedule.nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName).get());
    EXPECT_EQ("Coil:Cooling:Water", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CoolingCoilName).get());
    EXPECT_EQ("CoolReheat", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::DehumidificationControlType).get());
    EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad).get());
    EXPECT_EQ("Yes", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::RunonLatentLoad).get());
    EXPECT_EQ(1.0, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset).get());
    EXPECT_EQ("No", idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::EconomizerLockout).get());
    EXPECT_EQ(1.2, idfCoilSystem.getDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery).get());
    EXPECT_EQ(companionCoilUsedForHeatRecovery.nameString(),
              idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery).get());

    EXPECT_EQ(0, w.getObjectsByType(IddObjectType::Controller_WaterCoil)
                   .size());  // FIXME: what's creating this if i have it commented out in the addToNode method?

    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().nameString(), idfCoil.getString(Coil_Cooling_WaterFields::Name).get());
    EXPECT_EQ("Always On Discrete", idfCoil.getString(Coil_Cooling_WaterFields::AvailabilityScheduleName).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignWaterFlowRate).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignAirFlowRate).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletWaterTemperature).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirTemperature).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignInletAirHumidityRatio).get());
    EXPECT_EQ("Autosize", idfCoil.getString(Coil_Cooling_WaterFields::DesignOutletAirHumidityRatio).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterInletModelObject().get().nameString(),
              idfCoil.getString(Coil_Cooling_WaterFields::WaterInletNodeName).get());
    EXPECT_EQ(coilSystemCoolingWater.coolingCoil().waterOutletModelObject().get().nameString(),
              idfCoil.getString(Coil_Cooling_WaterFields::WaterOutletNodeName).get());
    EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirInletNodeName).get(),
              idfCoil.getString(Coil_Cooling_WaterFields::AirInletNodeName).get());
    EXPECT_EQ(idfCoilSystem.getString(CoilSystem_Cooling_WaterFields::AirOutletNodeName).get(),
              idfCoil.getString(Coil_Cooling_WaterFields::AirOutletNodeName).get());
    EXPECT_EQ("SimpleAnalysis", idfCoil.getString(Coil_Cooling_WaterFields::TypeofAnalysis).get());
    EXPECT_EQ("CrossFlow", idfCoil.getString(Coil_Cooling_WaterFields::HeatExchangerConfiguration).get());
    EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::CondensateCollectionWaterStorageTankName));
    EXPECT_TRUE(idfCoil.isEmpty(Coil_Cooling_WaterFields::DesignWaterTemperatureDifference));
  }
}
