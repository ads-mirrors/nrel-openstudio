/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#ifndef MODEL_PIPEADIABATICSTEAM_HPP
#define MODEL_PIPEADIABATICSTEAM_HPP

#include "ModelAPI.hpp"
#include "ModelObject.hpp"
#include "StraightComponent.hpp"
#include "Connection.hpp"

namespace openstudio {
namespace model {

  namespace detail {
    class PipeAdiabaticSteam_Impl;
  }  // namespace detail

  /** PipeAdiabaticSteam is an interface to the IDD object named "OS:Pipe:Adiabatic:Steam"
 *
 *  The purpose of this class is to simplify the construction and manipulation
 *  Pipe:Adiabatic:Steam objects in energyplus.
 */
  class MODEL_API PipeAdiabaticSteam : public StraightComponent
  {

   public:
    /** Constructs a new PipeAdiabatic object and places it inside the
   *  model.  The object is fully initialized with all companion objects.
   */
    explicit PipeAdiabaticSteam(const Model& model);

    virtual ~PipeAdiabaticSteam() override = default;
    // Default the copy and move operators because the virtual dtor is explicit
    PipeAdiabaticSteam(const PipeAdiabaticSteam& other) = default;
    PipeAdiabaticSteam(PipeAdiabaticSteam&& other) = default;
    PipeAdiabaticSteam& operator=(const PipeAdiabaticSteam&) = default;
    PipeAdiabaticSteam& operator=(PipeAdiabaticSteam&&) = default;

    unsigned inletPort() const;

    unsigned outletPort() const;

    static IddObjectType iddObjectType();

   protected:
    friend class Model;

    friend class openstudio::IdfObject;

    /// @cond

    using ImplType = detail::PipeAdiabaticSteam_Impl;

    explicit PipeAdiabaticSteam(std::shared_ptr<detail::PipeAdiabaticSteam_Impl> impl);

   private:
    REGISTER_LOGGER("openstudio.model.PipeAdiabaticSteam");

    /// @endcond
  };

  /** \relates PipeAdiabatic */
  using OptionalPipeAdiabaticSteam = boost::optional<PipeAdiabaticSteam>;

  /** \relates PipeAdiabatic */
  using PipeAdiabaticSteamVector = std::vector<PipeAdiabaticSteam>;

}  // namespace model
}  // namespace openstudio

#endif  // MODEL_PIPEADIABATICSTEAM_HPP
