/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "EnergyPlusFixture.hpp"

#include "../ForwardTranslator.hpp"

#include "../../model/Model.hpp"
#include "../../model/BoilerSteam.hpp"
#include "../../model/PlantLoop.hpp"
#include "../../model/CoilHeatingWater.hpp"
#include "../../model/CoilHeatingSteam.hpp"
#include "../../model/PipeAdiabatic.hpp"
#include "../../model/AirTerminalSingleDuctVAVReheat.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/ThermalZone.hpp"
#include "../../model/Space.hpp"
#include "../../model/AirLoopHVAC.hpp"

#include <utilities/idd/Boiler_Steam_FieldEnums.hxx>
#include <utilities/idd/Pipe_Adiabatic_FieldEnums.hxx>
#include <utilities/idd/Pipe_Adiabatic_Steam_FieldEnums.hxx>
#include <utilities/idd/AirTerminal_SingleDuct_VAV_Reheat_FieldEnums.hxx>

#include <utilities/idd/IddEnums.hxx>
#include "../../utilities/idf/IdfObject.hpp"
#include "../../utilities/idf/IdfObject_Impl.hpp"

#include "../../utilities/idf/WorkspaceObject.hpp"
#include "../../utilities/idf/WorkspaceObject_Impl.hpp"

using namespace openstudio::energyplus;
using namespace openstudio::model;
using namespace openstudio;

TEST_F(EnergyPlusFixture, ForwardTranslator_BoilerSteam) {
  Model m;

  PlantLoop plant_loop(m);

  BoilerSteam bs(m);

  EXPECT_TRUE(bs.setFuelType("Electricity"));
  EXPECT_TRUE(bs.setMaximumOperatingPressure(10.03));
  EXPECT_TRUE(bs.setTheoreticalEfficiency(0.5));
  EXPECT_TRUE(bs.setDesignOutletSteamTemperature(10.03));
  EXPECT_TRUE(bs.setNominalCapacity(10.03));
  EXPECT_TRUE(bs.setMinimumPartLoadRatio(1.0));
  EXPECT_TRUE(bs.setMaximumPartLoadRatio(1.0));
  EXPECT_TRUE(bs.setOptimumPartLoadRatio(1.0));
  EXPECT_TRUE(bs.setCoefficient1ofFuelUseFunctionofPartLoadRatioCurve(10.03));
  EXPECT_TRUE(bs.setCoefficient2ofFuelUseFunctionofPartLoadRatioCurve(10.03));
  EXPECT_TRUE(bs.setCoefficient3ofFuelUseFunctionofPartLoadRatioCurve(10.03));
  EXPECT_TRUE(bs.setSizingFactor(0.5));
  EXPECT_TRUE(bs.setEndUseSubcategory("SteamBoiler"));

  EXPECT_TRUE(plant_loop.addSupplyBranchForComponent(bs));

  ForwardTranslator ft;
  Workspace w = ft.translateModel(m);

  EXPECT_EQ(1u, w.getObjectsByType(IddObjectType::Boiler_Steam).size());
  ASSERT_EQ(0u, w.getObjectsByType(IddObjectType::Pipe_Adiabatic).size());
  ASSERT_EQ(6u, w.getObjectsByType(IddObjectType::Pipe_Adiabatic_Steam).size());
  IdfObject idf_bs = w.getObjectsByType(IddObjectType::Boiler_Steam)[0];

  EXPECT_EQ(bs.nameString(), idf_bs.getString(Boiler_SteamFields::Name, false).get());
  EXPECT_EQ("Electricity", idf_bs.getString(Boiler_SteamFields::FuelType, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::MaximumOperatingPressure, false).get());
  EXPECT_EQ(0.5, idf_bs.getDouble(Boiler_SteamFields::TheoreticalEfficiency, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::DesignOutletSteamTemperature, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::NominalCapacity, false).get());
  EXPECT_EQ(1.0, idf_bs.getDouble(Boiler_SteamFields::MinimumPartLoadRatio, false).get());
  EXPECT_EQ(1.0, idf_bs.getDouble(Boiler_SteamFields::MaximumPartLoadRatio, false).get());
  EXPECT_EQ(1.0, idf_bs.getDouble(Boiler_SteamFields::OptimumPartLoadRatio, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::Coefficient1ofFuelUseFunctionofPartLoadRatioCurve, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::Coefficient2ofFuelUseFunctionofPartLoadRatioCurve, false).get());
  EXPECT_EQ(10.03, idf_bs.getDouble(Boiler_SteamFields::Coefficient3ofFuelUseFunctionofPartLoadRatioCurve, false).get());
  EXPECT_EQ(bs.inletModelObject().get().nameString(), idf_bs.getString(Boiler_SteamFields::WaterInletNodeName, false).get());
  EXPECT_EQ(bs.outletModelObject().get().nameString(), idf_bs.getString(Boiler_SteamFields::SteamOutletNodeName, false).get());
  EXPECT_EQ(0.5, idf_bs.getDouble(Boiler_SteamFields::SizingFactor, false).get());
  EXPECT_EQ("SteamBoiler", idf_bs.getString(Boiler_SteamFields::EndUseSubcategory, false).get());
}

TEST_F(EnergyPlusFixture, ForwardTranslator_WaterAndSteam) {
  Model m;

  PlantLoop plant_loop(m);
  PipeAdiabatic pipe(m);
  EXPECT_TRUE(plant_loop.addSupplyBranchForComponent(pipe));

  BoilerSteam bs(m);
  CoilHeatingWater chw(m);

  EXPECT_TRUE(plant_loop.addSupplyBranchForComponent(bs));
  EXPECT_TRUE(plant_loop.addDemandBranchForComponent(chw));

  ForwardTranslator ft;
  Workspace w = ft.translateModel(m);

  EXPECT_EQ(2u, ft.errors().size());
  EXPECT_EQ("Did not translate Object of type 'OS:PlantLoop' and named 'Plant Loop 1' because there is a mix of Water and Steam components.",
            ft.errors().front().logMessage());

  EXPECT_EQ(0u, w.getObjectsByType(IddObjectType::PlantLoop).size());
  ASSERT_EQ(0u, w.getObjectsByType(IddObjectType::Pipe_Adiabatic).size());
  ASSERT_EQ(0u, w.getObjectsByType(IddObjectType::Pipe_Adiabatic_Steam).size());
}

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilHeatingSteam) {
  Model m;

  ThermalZone z(m);
  Space s(m);
  s.setThermalZone(z);

  Schedule sch = m.alwaysOnDiscreteSchedule();
  CoilHeatingSteam coil = CoilHeatingSteam(m, sch);
  AirTerminalSingleDuctVAVReheat atu(m, sch, coil);

  AirLoopHVAC a(m);
  a.addBranchForZone(z, atu);

  ForwardTranslator ft;
  Workspace w = ft.translateModel(m);

  WorkspaceObjectVector idf_atus(w.getObjectsByType(IddObjectType::AirTerminal_SingleDuct_VAV_Reheat));
  ASSERT_EQ(1u, idf_atus.size());
  WorkspaceObject idf_atu(idf_atus[0]);

  EXPECT_EQ("Coil:Heating:Steam", idf_atu.getString(AirTerminal_SingleDuct_VAV_ReheatFields::ReheatCoilObjectType).get());

  boost::optional<WorkspaceObject> woReheatCoil(idf_atu.getTarget(AirTerminal_SingleDuct_VAV_ReheatFields::ReheatCoilName));
  EXPECT_TRUE(woReheatCoil);
  EXPECT_EQ(woReheatCoil->iddObject().type(), IddObjectType::Coil_Heating_Steam);
  EXPECT_EQ("Coil Heating Steam 1", woReheatCoil->nameString());
}
