/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "ModelFixture.hpp"

#include "../OutputTableMonthly.hpp"
#include "../OutputTableMonthly_Impl.hpp"

using namespace openstudio;
using namespace openstudio::model;

TEST_F(ModelFixture, OutputTableMonthly_GettersSetters) {
  Model m;
  // TODO: Check regular Ctor arguments
  OutputTableMonthly outputTableMonthly(m);
  // TODO: Or if a UniqueModelObject (and make sure _Impl is included)
  // OutputTableMonthly outputTableMonthly = m.getUniqueModelObject<OutputTableMonthly>();

  outputTableMonthly.setName("My OutputTableMonthly");

  // Digits After Decimal: Required Integer
  EXPECT_TRUE(outputTableMonthly.setDigitsAfterDecimal(7));
  EXPECT_EQ(7, outputTableMonthly.digitsAfterDecimal());
  // Bad Value
  EXPECT_FALSE(outputTableMonthly.setDigitsAfterDecimal(-10));
  EXPECT_EQ(7, outputTableMonthly.digitsAfterDecimal());
}
