/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_OUTPUTTABLEMONTHLY_HPP
#define MODEL_OUTPUTTABLEMONTHLY_HPP

#include <model/ModelAPI.hpp>
#include "ModelObject.hpp"

namespace openstudio {
namespace model {

  namespace detail {

    class OutputTableMonthly_Impl;

  }  // namespace detail

  /** OutputTableMonthly is a ModelObject that wraps the OpenStudio IDD object 'OS:Output:Table:Monthly'. */
  class MODEL_API OutputTableMonthly : public ModelObject
  {
   public:
    /** @name Constructors and Destructors */
    //@{

    explicit OutputTableMonthly(const Model& model);

    virtual ~OutputTableMonthly() = default;
    // Default the copy and move operators because the virtual dtor is explicit
    OutputTableMonthly(const OutputTableMonthly& other) = default;
    OutputTableMonthly(OutputTableMonthly&& other) = default;
    OutputTableMonthly& operator=(const OutputTableMonthly&) = default;
    OutputTableMonthly& operator=(OutputTableMonthly&&) = default;

    //@}

    static IddObjectType iddObjectType();

    /** @name Getters */
    //@{

    int digitsAfterDecimal() const;

    // TODO: Handle this object's extensible fields.

    //@}
    /** @name Setters */
    //@{

    bool setDigitsAfterDecimal(int digitsAfterDecimal);

    // TODO: Handle this object's extensible fields.

    //@}
    /** @name Other */
    //@{

    //@}
   protected:
    /// @cond
    using ImplType = detail::OutputTableMonthly_Impl;

    explicit OutputTableMonthly(std::shared_ptr<detail::OutputTableMonthly_Impl> impl);

    friend class detail::OutputTableMonthly_Impl;
    friend class Model;
    friend class IdfObject;
    friend class openstudio::detail::IdfObject_Impl;
    /// @endcond
   private:
    REGISTER_LOGGER("openstudio.model.OutputTableMonthly");
  };

  /** \relates OutputTableMonthly*/
  using OptionalOutputTableMonthly = boost::optional<OutputTableMonthly>;

  /** \relates OutputTableMonthly*/
  using OutputTableMonthlyVector = std::vector<OutputTableMonthly>;

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_OUTPUTTABLEMONTHLY_HPP
