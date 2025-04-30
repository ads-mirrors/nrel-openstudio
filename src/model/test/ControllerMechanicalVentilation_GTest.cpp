/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include <gtest/gtest.h>
#include "ModelFixture.hpp"
#include "../Model.hpp"
#include "../ControllerMechanicalVentilation.hpp"
#include "../ControllerOutdoorAir.hpp"
#include "../AirLoopHVAC.hpp"
#include "../AirLoopHVACOutdoorAirSystem.hpp"
#include "../DesignSpecificationOutdoorAir.hpp"
#include "../Node.hpp"
#include "../Space.hpp"
#include "../ThermalZone.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, ControllerMechanicalVentilation) {
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  ASSERT_EXIT(
    {
      model::Model m;

      model::ControllerMechanicalVentilation controller(m);
      exit(0);
    },
    ::testing::ExitedWithCode(0), "");
}

TEST_F(ModelFixture, ControllerMechanicalVentilation_SystemOutdoorAirMethod) {

  Model m;
  ControllerMechanicalVentilation c(m);
  EXPECT_TRUE(c.isSystemOutdoorAirMethodDefaulted());
  EXPECT_EQ("Standard62.1VentilationRateProcedure", c.systemOutdoorAirMethod());
}

TEST_F(ModelFixture, ControllerMechanicalVentilation_hasZonesWithDesignSpecificationOutdoorAir) {

  Model m;

  AirLoopHVAC a(m);

  ControllerOutdoorAir oa_controller(m);
  AirLoopHVACOutdoorAirSystem oa_system(m, oa_controller);
  auto supplyInletNode = a.supplyInletNode();
  oa_system.addToNode(supplyInletNode);

  ControllerMechanicalVentilation controller_mv = oa_controller.controllerMechanicalVentilation();
  EXPECT_FALSE(controller_mv.hasZonesWithDesignSpecificationOutdoorAir());

  ThermalZone z(m);
  Space space1(m);
  EXPECT_TRUE(space1.setThermalZone(z));
  EXPECT_EQ(0, z.spacesWithDesignSpecificationOutdoorAir().size());

  EXPECT_TRUE(a.addBranchForZone(z));
  EXPECT_EQ(1, a.thermalZones().size());
  EXPECT_EQ(1, z.spaces().size());
  EXPECT_EQ(0, z.spacesWithDesignSpecificationOutdoorAir().size());
  EXPECT_FALSE(controller_mv.hasZonesWithDesignSpecificationOutdoorAir());

  DesignSpecificationOutdoorAir dsoa_space1(m);
  EXPECT_TRUE(space1.setDesignSpecificationOutdoorAir(dsoa_space1));
  EXPECT_EQ(1, z.spaces().size());
  EXPECT_EQ(1, z.spacesWithDesignSpecificationOutdoorAir().size());
  EXPECT_TRUE(controller_mv.hasZonesWithDesignSpecificationOutdoorAir());
}
