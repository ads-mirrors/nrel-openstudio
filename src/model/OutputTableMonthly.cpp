/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OutputTableMonthly.hpp"
#include "OutputTableMonthly_Impl.hpp"

#include "../utilities/core/Assert.hpp"

#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/OS_Output_Table_Monthly_FieldEnums.hxx>

namespace openstudio {
namespace model {

  namespace detail {

    OutputTableMonthly_Impl::OutputTableMonthly_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(idfObject, model, keepHandle) {
      OS_ASSERT(idfObject.iddObject().type() == OutputTableMonthly::iddObjectType());
    }

    OutputTableMonthly_Impl::OutputTableMonthly_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(other, model, keepHandle) {
      OS_ASSERT(other.iddObject().type() == OutputTableMonthly::iddObjectType());
    }

    OutputTableMonthly_Impl::OutputTableMonthly_Impl(const OutputTableMonthly_Impl& other, Model_Impl* model, bool keepHandle)
      : ModelObject_Impl(other, model, keepHandle) {}

    const std::vector<std::string>& OutputTableMonthly_Impl::outputVariableNames() const {
      static std::vector<std::string> result;
      if (result.empty()) {
      }
      return result;
    }

    IddObjectType OutputTableMonthly_Impl::iddObjectType() const {
      return OutputTableMonthly::iddObjectType();
    }

    int OutputTableMonthly_Impl::digitsAfterDecimal() const {
      boost::optional<int> value = getInt(OS_Output_Table_MonthlyFields::DigitsAfterDecimal, true);
      OS_ASSERT(value);
      return value.get();
    }

    bool OutputTableMonthly_Impl::setDigitsAfterDecimal(int digitsAfterDecimal) {
      const bool result = setInt(OS_Output_Table_MonthlyFields::DigitsAfterDecimal, digitsAfterDecimal);
      return result;
    }

  }  // namespace detail

  OutputTableMonthly::OutputTableMonthly(const Model& model) : ModelObject(OutputTableMonthly::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::OutputTableMonthly_Impl>());

    // TODO: consider adding (overloaded or not) explicit ctors taking required objects as argument

    // TODO: Appropriately handle the following required object-list fields.
    bool ok = true;
    // ok = setDigitsAfterDecimal();
    OS_ASSERT(ok);
  }

  IddObjectType OutputTableMonthly::iddObjectType() {
    return {IddObjectType::OS_Output_Table_Monthly};
  }

  int OutputTableMonthly::digitsAfterDecimal() const {
    return getImpl<detail::OutputTableMonthly_Impl>()->digitsAfterDecimal();
  }

  bool OutputTableMonthly::setDigitsAfterDecimal(int digitsAfterDecimal) {
    return getImpl<detail::OutputTableMonthly_Impl>()->setDigitsAfterDecimal(digitsAfterDecimal);
  }

  /// @cond
  OutputTableMonthly::OutputTableMonthly(std::shared_ptr<detail::OutputTableMonthly_Impl> impl) : ModelObject(std::move(impl)) {}
  /// @endcond

}  // namespace model
}  // namespace openstudio
