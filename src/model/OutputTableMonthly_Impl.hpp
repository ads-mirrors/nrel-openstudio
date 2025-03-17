/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_OUTPUTTABLEMONTHLY_IMPL_HPP
#define MODEL_OUTPUTTABLEMONTHLY_IMPL_HPP

#include <model/ModelAPI.hpp>
#include "ModelObject_Impl.hpp"

namespace openstudio {
namespace model {

  namespace detail {

    /** OutputTableMonthly_Impl is a ModelObject_Impl that is the implementation class for OutputTableMonthly.*/
    class MODEL_API OutputTableMonthly_Impl : public ModelObject_Impl
    {
     public:
      /** @name Constructors and Destructors */
      //@{

      OutputTableMonthly_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle);

      OutputTableMonthly_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle);

      OutputTableMonthly_Impl(const OutputTableMonthly_Impl& other, Model_Impl* model, bool keepHandle);

      virtual ~OutputTableMonthly_Impl() = default;

      //@}
      /** @name Virtual Methods */
      //@{

      virtual const std::vector<std::string>& outputVariableNames() const override;

      virtual IddObjectType iddObjectType() const override;

      //@}
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
     private:
      REGISTER_LOGGER("openstudio.model.OutputTableMonthly");
    };

  }  // namespace detail

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_OUTPUTTABLEMONTHLY_IMPL_HPP
