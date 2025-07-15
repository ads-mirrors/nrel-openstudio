/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "PipeAdiabaticSteam.hpp"
#include "PipeAdiabaticSteam_Impl.hpp"
#include "Node.hpp"
#include "Node_Impl.hpp"
#include "AirLoopHVAC.hpp"
#include "AirLoopHVAC_Impl.hpp"
#include "Model.hpp"

#include "../utilities/core/Assert.hpp"
#include "../utilities/core/Compare.hpp"
#include "../utilities/data/DataEnums.hpp"

#include <utilities/idd/OS_Pipe_Adiabatic_Steam_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

namespace openstudio {

namespace model {

  namespace detail {

    PipeAdiabaticSteam_Impl::PipeAdiabaticSteam_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(idfObject, model, keepHandle) {
      OS_ASSERT(idfObject.iddObject().type() == PipeAdiabaticSteam::iddObjectType());
    }

    PipeAdiabaticSteam_Impl::PipeAdiabaticSteam_Impl(const openstudio::detail::WorkspaceObject_Impl& other, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {
      OS_ASSERT(other.iddObject().type() == PipeAdiabaticSteam::iddObjectType());
    }

    PipeAdiabaticSteam_Impl::PipeAdiabaticSteam_Impl(const PipeAdiabaticSteam_Impl& other, Model_Impl* model, bool keepHandle)
      : StraightComponent_Impl(other, model, keepHandle) {}

    const std::vector<std::string>& PipeAdiabaticSteam_Impl::outputVariableNames() const {
      static const std::vector<std::string> result;
      // Not appropriate: no specific output
      return result;
    }

    IddObjectType PipeAdiabaticSteam_Impl::iddObjectType() const {
      return PipeAdiabaticSteam::iddObjectType();
    }

    unsigned PipeAdiabaticSteam_Impl::inletPort() const {
      return OS_Pipe_Adiabatic_SteamFields::InletNodeName;
    }

    unsigned PipeAdiabaticSteam_Impl::outletPort() const {
      return OS_Pipe_Adiabatic_SteamFields::OutletNodeName;
    }

    bool PipeAdiabaticSteam_Impl::addToNode(Node& node) {
      if (boost::optional<PlantLoop> plant = node.plantLoop()) {
        return StraightComponent_Impl::addToNode(node);
      }

      return false;
    }

    ComponentType PipeAdiabaticSteam_Impl::componentType() const {
      return ComponentType::None;
    }

    std::vector<FuelType> PipeAdiabaticSteam_Impl::coolingFuelTypes() const {
      return {};
    }

    std::vector<FuelType> PipeAdiabaticSteam_Impl::heatingFuelTypes() const {
      return {};
    }

    std::vector<AppGFuelType> PipeAdiabaticSteam_Impl::appGHeatingFuelTypes() const {
      return {};
    }

  }  // namespace detail

  // create a new PipeAdiabaticSteam object in the model's workspace
  PipeAdiabaticSteam::PipeAdiabaticSteam(const Model& model) : StraightComponent(PipeAdiabaticSteam::iddObjectType(), model) {
    OS_ASSERT(getImpl<detail::PipeAdiabaticSteam_Impl>());
  }

  unsigned PipeAdiabaticSteam::inletPort() const {
    return getImpl<detail::PipeAdiabaticSteam_Impl>()->inletPort();
  }

  unsigned PipeAdiabaticSteam::outletPort() const {
    return getImpl<detail::PipeAdiabaticSteam_Impl>()->outletPort();
  }

  PipeAdiabaticSteam::PipeAdiabaticSteam(std::shared_ptr<detail::PipeAdiabaticSteam_Impl> p) : StraightComponent(std::move(p)) {}

  IddObjectType PipeAdiabaticSteam::iddObjectType() {
    IddObjectType result(IddObjectType::OS_Pipe_Adiabatic_Steam);
    return result;
  }

}  // namespace model
}  // namespace openstudio
