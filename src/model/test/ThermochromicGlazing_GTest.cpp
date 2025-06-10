/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../ThermochromicGlazing.hpp"
#include "../ThermochromicGlazing_Impl.hpp"
#include "../StandardGlazing.hpp"
#include "../StandardGlazing_Impl.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, ThermochromicGlazing_GettersSetters) {
  Model m;
  ThermochromicGlazing thermochromicGlazing(m);

  thermochromicGlazing.setName("My ThermochromicGlazing");
}

TEST_F(ModelFixture, ThermochromicGlazing_ThermochromicGroups) {

  Model m;

  StandardGlazing standardGlazing(m);

  EXPECT_NO_THROW(ThermochromicGroup(standardGlazing, -10.0));

  ThermochromicGlazing thermochromicGlazing(m);

  EXPECT_EQ(0, thermochromicGlazing.numberofThermochromicGroups());
  EXPECT_TRUE(thermochromicGlazing.addThermochromicGroup(standardGlazing, 0.0));
  EXPECT_EQ(1, thermochromicGlazing.numberofThermochromicGroups());

  StandardGlazing standardGlazing2(m);
  auto group2 = ThermochromicGroup(standardGlazing2, 10.0);
  EXPECT_TRUE(thermochromicGlazing.addThermochromicGroup(group2));
  EXPECT_EQ(2, thermochromicGlazing.numberofThermochromicGroups());

  boost::optional<ThermochromicGroup> group_ = thermochromicGlazing.getThermochromicGroup(1U);
  ASSERT_TRUE(group_);
  EXPECT_EQ(10.0, group_->opticalDataTemperature());
  EXPECT_EQ(standardGlazing2, group_->standardGlazing());

  // Test that you cannot add the same group twice (based on StandardGlazing only), but that instead it'll overwrite the values
  auto group2bis = ThermochromicGroup(standardGlazing2, 20.0);
  // Test that we can locate an existing Group correctly (comparing on StandardGlazing only, ignoring opticalDataTemperature)
  boost::optional<unsigned> _existingIndex = thermochromicGlazing.thermochromicGroupIndex(group2bis);
  ASSERT_TRUE(_existingIndex);
  EXPECT_EQ(1U, _existingIndex.get());
  // Now call add, which should jut override it
  EXPECT_TRUE(thermochromicGlazing.addThermochromicGroup(group2bis));
  // Should still have the same number
  EXPECT_EQ(2, thermochromicGlazing.numberofThermochromicGroups());
  // Retrieve it again, making sure it changed only the Optical Data Temperature
  boost::optional<ThermochromicGroup> group2_ = thermochromicGlazing.getThermochromicGroup(1U);
  ASSERT_TRUE(group2_);
  EXPECT_EQ(20.0, group2_->opticalDataTemperature());
  EXPECT_EQ(standardGlazing2, group2_->standardGlazing());

  // Test that you cannot get a group by an index that's too high
  EXPECT_FALSE(thermochromicGlazing.getThermochromicGroup(thermochromicGlazing.numberofThermochromicGroups()));

  // Test that you cannot find a group if it doesn't exist
  StandardGlazing standardGlazing3(m);
  auto group3 = ThermochromicGroup(standardGlazing3, 30.0);
  EXPECT_FALSE(thermochromicGlazing.thermochromicGroupIndex(group3));
  EXPECT_TRUE(thermochromicGlazing.addThermochromicGroup(group3));
  EXPECT_TRUE(thermochromicGlazing.thermochromicGroupIndex(group3));
  EXPECT_EQ(3, thermochromicGlazing.numberofThermochromicGroups());

  // Remove a group
  EXPECT_FALSE(thermochromicGlazing.removeThermochromicGroup(1000));
  EXPECT_EQ(3, thermochromicGlazing.numberofThermochromicGroups());

  EXPECT_TRUE(thermochromicGlazing.removeThermochromicGroup(1));
  EXPECT_EQ(2, thermochromicGlazing.numberofThermochromicGroups());

  // check that remaining blocks moved correctly
  std::vector<ThermochromicGroup> groups = thermochromicGlazing.thermochromicGroups();
  EXPECT_EQ(2U, groups.size());
  EXPECT_EQ(0.0, groups[0].opticalDataTemperature());
  EXPECT_EQ(standardGlazing, groups[0].standardGlazing());

  EXPECT_EQ(30.0, groups[1].opticalDataTemperature());
  EXPECT_EQ(standardGlazing3, groups[1].standardGlazing());

  // more remove checking
  thermochromicGlazing.removeAllThermochromicGroups();
  EXPECT_EQ(0, thermochromicGlazing.numberofThermochromicGroups());
  EXPECT_FALSE(thermochromicGlazing.removeThermochromicGroup(0));
  EXPECT_EQ(0, thermochromicGlazing.numberofThermochromicGroups());

  // Via a vector
  EXPECT_TRUE(thermochromicGlazing.addThermochromicGroups({group2bis, group3}));
  groups = thermochromicGlazing.thermochromicGroups();
  EXPECT_EQ(2U, groups.size());
  EXPECT_EQ(group2bis.standardGlazing(), groups[0].standardGlazing());
  EXPECT_EQ(group2bis.opticalDataTemperature(), groups[0].opticalDataTemperature());

  EXPECT_EQ(group3.standardGlazing(), groups[1].standardGlazing());
  EXPECT_EQ(group3.opticalDataTemperature(), groups[1].opticalDataTemperature());
}
