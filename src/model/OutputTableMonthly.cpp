/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "OutputTableMonthly.hpp"
#include "OutputTableMonthly_Impl.hpp"

#include "ModelExtensibleGroup.hpp"

#include "../utilities/core/Assert.hpp"

#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/OS_Output_Table_Monthly_FieldEnums.hxx>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace openstudio {
namespace model {

  MonthlyVariableGroup::MonthlyVariableGroup(std::string variableOrMeterName, std::string aggregationType)
    : m_variableOrMeterName(std::move(variableOrMeterName)), m_aggregationType(std::move(aggregationType)) {
    if (m_variableOrMeterName.empty()) {
      LOG_AND_THROW("Variable or Meter Name can't be empty");
    }
    if (!OutputTableMonthly::isAggregationTypeValid(m_aggregationType)) {
      LOG_AND_THROW(fmt::format("Invalid aggregation type '{}': {}", m_aggregationType, OutputTableMonthly::aggregationTypeValues()));
    }
  }

  std::string MonthlyVariableGroup::variableOrMeterName() const {
    return m_variableOrMeterName;
  }

  std::string MonthlyVariableGroup::aggregationType() const {
    return m_aggregationType;
  }

  bool MonthlyVariableGroup::operator==(const MonthlyVariableGroup& other) const {
    return openstudio::istringEqual(variableOrMeterName(), other.variableOrMeterName())
           && openstudio::istringEqual(aggregationType(), other.aggregationType());
  }

  bool MonthlyVariableGroup::operator!=(const MonthlyVariableGroup& other) const {
    return (!operator==(other));
  }

  bool MonthlyVariableGroup::isAggregationTypeAdvanced() const {
    return OutputTableMonthly::isAggregationTypeAdvanced(m_aggregationType);
  }

  std::ostream& operator<<(std::ostream& out, const openstudio::model::MonthlyVariableGroup& monthlyVariableGroup) {
    out << "(Output Variable or Meter = '" << monthlyVariableGroup.variableOrMeterName() << "', "
        << "Aggregation Type = '" << monthlyVariableGroup.aggregationType() << "')";
    return out;
  }

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

    unsigned int OutputTableMonthly_Impl::numberofMonthlyVariableGroups() const {
      return numExtensibleGroups();
    }

    std::vector<MonthlyVariableGroup> OutputTableMonthly_Impl::monthlyVariableGroups() const {
      std::vector<MonthlyVariableGroup> result;

      for (unsigned i = 0; i < numberofMonthlyVariableGroups(); ++i) {

        boost::optional<MonthlyVariableGroup> group_ = getMonthlyVariableGroup(i);

        // getMonthlyVariableGroup is responsible for handling error and issuing Error log messages.
        // Here we add it to the result array if it worked, and if it didn't, we keep going
        // We just issue a message about index so user can delete it easily
        if (group_) {
          result.push_back(std::move(*group_));
        } else {
          LOG(Error, briefDescription() << " has an invalid MonthlyVariableGroup group at index " << i);
        }
      }

      return result;
    }

    boost::optional<unsigned> OutputTableMonthly_Impl::monthlyVariableGroupIndex(const MonthlyVariableGroup& monthlyVariableGroup) const {
      const std::vector<MonthlyVariableGroup> groups = monthlyVariableGroups();
      auto it = std::find(groups.cbegin(), groups.cend(), monthlyVariableGroup);
      if (it != groups.end()) {
        return std::distance(groups.cbegin(), it);
      }
      return boost::none;
    }

    boost::optional<MonthlyVariableGroup> OutputTableMonthly_Impl::getMonthlyVariableGroup(unsigned groupIndex) const {
      if (groupIndex >= numberofMonthlyVariableGroups()) {
        LOG(Error, "Asked to get MonthlyVariableGroup with index " << groupIndex << ", but " << briefDescription() << " has just "
                                                                   << numberofMonthlyVariableGroups() << " MonthlyVariableGroups.");
        return boost::none;
      }
      auto group = getExtensibleGroup(groupIndex).cast<ModelExtensibleGroup>();

      boost::optional<std::string> variableOrMeterName_ = group.getString(OS_Output_Table_MonthlyExtensibleFields::VariableorMeterName);
      boost::optional<std::string> aggregationType_ = group.getString(OS_Output_Table_MonthlyExtensibleFields::AggregationTypeforVariableorMeter);

      if (!variableOrMeterName_) {
        LOG(Error, "Could not retrieve VariableorMeterName_ for extensible group " << group.groupIndex() << ".");
        return boost::none;
      }
      if (!aggregationType_) {
        LOG(Error, "Could not retrieve Aggregation Type for extensible group " << group.groupIndex() << ".");
        return boost::none;
      }

      return {MonthlyVariableGroup(std::move(*variableOrMeterName_), std::move(*aggregationType_))};
    }

    bool OutputTableMonthly_Impl::addMonthlyVariableGroup(const MonthlyVariableGroup& monthlyVariableGroup) {
      if (!monthlyVariableGroup.isAggregationTypeAdvanced()) {
        boost::optional<unsigned> existingIndex_ = monthlyVariableGroupIndex(monthlyVariableGroup);
        if (existingIndex_) {
          LOG(Warn, "For " << briefDescription()
                           << ", MonthlyVariableGroup already exists and is not an Advanced Aggregation Type: " << monthlyVariableGroup);
          return false;
        }
      }
      IdfExtensibleGroup eg = pushExtensibleGroup(StringVector());
      eg.setString(OS_Output_Table_MonthlyExtensibleFields::VariableorMeterName, monthlyVariableGroup.variableOrMeterName());
      eg.setString(OS_Output_Table_MonthlyExtensibleFields::AggregationTypeforVariableorMeter, monthlyVariableGroup.aggregationType());
      return true;
    }

    bool OutputTableMonthly_Impl::addMonthlyVariableGroups(const std::vector<MonthlyVariableGroup>& monthlyVariableGroups) {
      bool result = true;

      for (const auto& monthlyVariableGroup : monthlyVariableGroups) {
        bool thisResult = addMonthlyVariableGroup(monthlyVariableGroup);
        if (!thisResult) {
          LOG(Error, "Could not add MonthlyVariableGroup " << monthlyVariableGroup << " to " << briefDescription() << ". Continuing with others.");
          // OS_ASSERT(false);
          // result = false;
        }
      }

      return result;
    }

    bool OutputTableMonthly_Impl::removeMonthlyVariableGroup(unsigned groupIndex) {
      bool result = false;

      unsigned int num = numberofMonthlyVariableGroups();
      if (groupIndex < num) {
        getObject<ModelObject>().eraseExtensibleGroup(groupIndex);
        result = true;
      }
      return result;
    }

    void OutputTableMonthly_Impl::removeAllMonthlyVariableGroups() {
      getObject<ModelObject>().clearExtensibleGroups();
    }

  }  // namespace detail

  OutputTableMonthly::OutputTableMonthly(const Model& model) : ModelObject(OutputTableMonthly::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::OutputTableMonthly_Impl>());
    bool ok = true;
    ok = setDigitsAfterDecimal(2);
    OS_ASSERT(ok);
  }

  std::vector<std::string> OutputTableMonthly::aggregationTypeValues() {
    IddObject obj = IddFactory::instance().getObject(iddObjectType()).get();
    // Return IddKeyNames in extensible portion
    return getIddKeyNames(obj, obj.numFields() + OS_Output_Table_MonthlyExtensibleFields::AggregationTypeforVariableorMeter);
  }

  std::vector<std::string> OutputTableMonthly::validAggregationTypes() {
    return aggregationTypeValues();
  }

  bool OutputTableMonthly::isAggregationTypeValid(const std::string& aggregationType) {
    const auto vals = aggregationTypeValues();
    return std::find_if(vals.cbegin(), vals.cend(),
                        [&aggregationType](const std::string& choice) { return openstudio::istringEqual(aggregationType, choice); })
           != vals.cend();
  }

  bool OutputTableMonthly::isAggregationTypeAdvanced(const std::string& aggregationType) {
    auto const& vals = advancedAggregationTypes();
    return std::find_if(vals.cbegin(), vals.cend(),
                        [&aggregationType](const std::string& choice) { return openstudio::istringEqual(aggregationType, choice); })
           != vals.cend();
  }

  const std::vector<std::string>& OutputTableMonthly::advancedAggregationTypes() {
    static std::vector<std::string> result;
    if (result.empty()) {
      result = {"ValueWhenMaximumOrMinimum", "SumOrAverageDuringHoursShown", "MaximumDuringHoursShown", "MinimumDuringHoursShown"};
    }
    return result;
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

  std::vector<MonthlyVariableGroup> OutputTableMonthly::monthlyVariableGroups() const {
    return getImpl<detail::OutputTableMonthly_Impl>()->monthlyVariableGroups();
  }

  unsigned int OutputTableMonthly::numberofMonthlyVariableGroups() const {
    return getImpl<detail::OutputTableMonthly_Impl>()->numberofMonthlyVariableGroups();
  }

  boost::optional<unsigned> OutputTableMonthly::monthlyVariableGroupIndex(const MonthlyVariableGroup& monthlyVariableGroup) const {
    return getImpl<detail::OutputTableMonthly_Impl>()->monthlyVariableGroupIndex(monthlyVariableGroup);
  }

  boost::optional<MonthlyVariableGroup> OutputTableMonthly::getMonthlyVariableGroup(unsigned groupIndex) const {
    return getImpl<detail::OutputTableMonthly_Impl>()->getMonthlyVariableGroup(groupIndex);
  }

  bool OutputTableMonthly::addMonthlyVariableGroup(const MonthlyVariableGroup& monthlyVariableGroup) {
    return getImpl<detail::OutputTableMonthly_Impl>()->addMonthlyVariableGroup(monthlyVariableGroup);
  }

  bool OutputTableMonthly::addMonthlyVariableGroup(std::string variableOrMeterName, std::string aggregationType) {
    return addMonthlyVariableGroup(MonthlyVariableGroup(std::move(variableOrMeterName), std::move(aggregationType)));
  }

  bool OutputTableMonthly::addMonthlyVariableGroups(const std::vector<MonthlyVariableGroup>& monthlyVariableGroups) {
    return getImpl<detail::OutputTableMonthly_Impl>()->addMonthlyVariableGroups(monthlyVariableGroups);
  }

  bool OutputTableMonthly::removeMonthlyVariableGroup(unsigned groupIndex) {
    return getImpl<detail::OutputTableMonthly_Impl>()->removeMonthlyVariableGroup(groupIndex);
  }

  void OutputTableMonthly::removeAllMonthlyVariableGroups() {
    getImpl<detail::OutputTableMonthly_Impl>()->removeAllMonthlyVariableGroups();
  }

  /// @cond
  OutputTableMonthly::OutputTableMonthly(std::shared_ptr<detail::OutputTableMonthly_Impl> impl) : ModelObject(std::move(impl)) {}
  /// @endcond

}  // namespace model
}  // namespace openstudio
