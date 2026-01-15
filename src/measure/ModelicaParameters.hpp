/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Energy Innovation, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MEASURE_MODELICAPARAMETERS_HPP
#define MEASURE_MODELICAPARAMETERS_HPP

#include "MeasureAPI.hpp"
#include <memory>
#include <string>
#include <vector>

namespace openstudio {

namespace measure {

  namespace detail {
    class ModelicaParameter_Impl;
    class ModelicaParameters_Impl;
  }  // namespace detail

  class MEASURE_API ModelicaParameter
  {
   public:
    ModelicaParameter(const std::string& model, const std::string& key, const std::string& value);

    std::string model() const;
    std::string key() const;

    std::string value() const;
    void setValue(const std::string& value);

   private:
    std::shared_ptr<detail::ModelicaParameter_Impl> m_impl;
  };

  /** ModelicaParameter stores a collection ModelicaParameter
   *  and provides methods to access them.*/
  class MEASURE_API ModelicaParameters
  {
   public:
    ModelicaParameters();

    std::string getParameterValue(const std::string& model, const std::string& key) const;
    void setParameterValue(const std::string& model, const std::string& key, const std::string& value);

    std::vector<ModelicaParameter> getAllParameters() const;
    void clearAllParameters();

   private:
    std::shared_ptr<detail::ModelicaParameters_Impl> m_impl;
  };

}  // namespace measure
}  // namespace openstudio

#endif  // MEASURE_MODELICAPARAMETERS_HPP
