/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "../ForwardTranslator.hpp"
#include "../../model/Model.hpp"
#include "../../model/PipeAdiabatic.hpp"
#include "../../model/PipeAdiabatic_Impl.hpp"
#include "../../utilities/idf/IdfExtensibleGroup.hpp"
#include "../../utilities/idf/Workspace.hpp"
#include "../../utilities/idf/WorkspaceObjectOrder.hpp"
#include "../../utilities/core/Logger.hpp"
#include <utilities/idd/Pipe_Adiabatic_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include "../../model/BoilerSteam.hpp"
#include "../../model/BoilerSteam_Impl.hpp"
#include "../../model/DistrictHeatingSteam.hpp"
#include "../../model/DistrictHeatingSteam_Impl.hpp"
#include "../../model/PumpVariableSpeedCondensate.hpp"
#include "../../model/PumpVariableSpeedCondensate_Impl.hpp"
#include "../../model/CoilHeatingSteam.hpp"
#include "../../model/CoilHeatingSteam_Impl.hpp"
#include "../../model/CoilHeatingSteamBaseboardRadiant.hpp"
#include "../../model/CoilHeatingSteamBaseboardRadiant_Impl.hpp"
#include "../../model/Node.hpp"
#include "../../model/Node_Impl.hpp"
#include "../../model/Splitter.hpp"
#include "../../model/Splitter_Impl.hpp"
#include "../../model/Mixer.hpp"
#include "../../model/Mixer_Impl.hpp"
#include "../../model/PlantLoop.hpp"
#include "../../model/PlantLoop_Impl.hpp"

using namespace openstudio::model;

namespace openstudio {

namespace energyplus {

  boost::optional<IdfObject> ForwardTranslator::translatePipeAdiabatic(PipeAdiabatic& modelObject) {
    OptionalString s;
    OptionalDouble d;
    OptionalModelObject temp;

    // PipeAdiabatic or PipeAdiabaticSteam
    bool hasWater = false;
    bool hasSteam = false;

  std::vector<PlantLoop> plantLoops = modelObject.model().getConcreteModelObjects<PlantLoop>();
    for (const auto& plantLoop : plantLoops) {
      std::vector<ModelObject> components = plantLoop.components();
      for (auto& component : components) {
        if (component.handle() == modelObject.handle()) {

          for (auto& component : components) {
            if (component.optionalCast<PipeAdiabatic>()
                || component.optionalCast<Node>()
                || component.optionalCast<Mixer>()
                || component.optionalCast<Splitter>()) {
              // no-op        
            } else if (component.optionalCast<BoilerSteam>()
                || component.optionalCast<DistrictHeatingSteam>()
                || component.optionalCast<PumpVariableSpeedCondensate>()
                || component.optionalCast<CoilHeatingSteam>()
                || component.optionalCast<CoilHeatingSteamBaseboardRadiant>()) {
              hasSteam = true;
            } else {
              hasWater = true;
            }
          }

        }
      }
    }
 
    boost::optional<IdfObject> idfObject;
    if (hasWater) {
      idfObject = IdfObject(IddObjectType::Pipe_Adiabatic);
    } else if (hasSteam) {
      idfObject = IdfObject(IddObjectType::Pipe_Adiabatic_Steam);
    }

    m_idfObjects.push_back(*idfObject);

    ///////////////////////////////////////////////////////////////////////////
    // Field: Name ////////////////////////////////////////////////////////////
    s = modelObject.name();
    if (s) {
      idfObject->setName(*s);
    }
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Inlet Node Name ////////////////////////////////////////////////////
    temp = modelObject.inletModelObject();
    if (temp) {
      s = temp->name();
      if (s) {
        idfObject->setString(openstudio::Pipe_AdiabaticFields::InletNodeName, *s);
      }
    }
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Outlet Node Name ///////////////////////////////////////////////////
    temp = modelObject.outletModelObject();
    if (temp) {
      s = temp->name();
      if (s) {
        idfObject->setString(openstudio::Pipe_AdiabaticFields::OutletNodeName, *s);
      }
    }
    ///
    ////////////////////////////////////////////////////////////////////////

    return idfObject;
  }

}  // namespace energyplus

}  // namespace openstudio
