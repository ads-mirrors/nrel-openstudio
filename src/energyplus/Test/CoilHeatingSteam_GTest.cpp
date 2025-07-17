/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "EnergyPlusFixture.hpp"

#include "../ForwardTranslator.hpp"

#include "../../model/Model.hpp"
#include "../../model/CoilHeatingSteam.hpp"
#include "../../model/AirTerminalSingleDuctVAVReheat.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/ThermalZone.hpp"
#include "../../model/Space.hpp"
#include "../../model/AirLoopHVAC.hpp"

#include <utilities/idd/AirTerminal_SingleDuct_VAV_Reheat_FieldEnums.hxx>

#include <utilities/idd/IddEnums.hxx>
#include "../../utilities/idf/IdfObject.hpp"
#include "../../utilities/idf/IdfObject_Impl.hpp"

#include "../../utilities/idf/WorkspaceObject.hpp"
#include "../../utilities/idf/WorkspaceObject_Impl.hpp"

using namespace openstudio::energyplus;
using namespace openstudio::model;
using namespace openstudio;

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
