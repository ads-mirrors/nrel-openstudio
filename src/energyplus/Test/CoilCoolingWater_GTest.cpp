/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "EnergyPlusFixture.hpp"

#include "../ForwardTranslator.hpp"

#include "../../model/Model.hpp"
#include "../../model/CoilCoolingWater.hpp"
#include "../../model/CoilCoolingWater_Impl.hpp"

#include "../../model/AirLoopHVAC.hpp"
#include "../../model/AirLoopHVACUnitarySystem.hpp"
#include "../../model/Node.hpp"

#include "../../utilities/idf/IdfObject.hpp"
#include "../../utilities/idf/IdfObject_Impl.hpp"
#include "../../utilities/idf/WorkspaceObject.hpp"
#include "../../utilities/idf/WorkspaceObject_Impl.hpp"
#include "../../utilities/idf/IdfExtensibleGroup.hpp"
#include "../../utilities/idf/WorkspaceExtensibleGroup.hpp"

#include <utilities/idd/Coil_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/CoilSystem_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/AirLoopHVAC_FieldEnums.hxx>
#include <utilities/idd/BranchList_FieldEnums.hxx>
#include <utilities/idd/Branch_FieldEnums.hxx>
#include <utilities/idd/AirLoopHVAC_UnitarySystem_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::energyplus;
using namespace openstudio::model;
using namespace openstudio;

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilCoolingWater_Unitary) {
  Model m;
  CoilCoolingWater coil(m);

  ForwardTranslator ft;
  Workspace w = ft.translateModel(m);

  // No CoilSystem:Cooling:DX wrapper needed, it's inside a unitary
  EXPECT_EQ(0, w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water).size());
}

TEST_F(EnergyPlusFixture, ForwardTranslator_CoilCoolingWater_AirLoopHVAC) {

  ForwardTranslator ft;

  Model m;
  CoilCoolingWater coil(m);

  Workspace w = ft.translateModel(m);

  EXPECT_EQ(1, w.getObjectsByType(IddObjectType::CoilSystem_Cooling_Water).size());
}
