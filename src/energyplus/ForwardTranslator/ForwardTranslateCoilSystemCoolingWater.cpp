/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "../ForwardTranslator.hpp"
#include "../../model/Model.hpp"
#include "../../model/CoilSystemCoolingWater.hpp"
#include "../../model/CoilSystemCoolingWater_Impl.hpp"
#include "../../model/Node.hpp"
#include "../../model/Node_Impl.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/CoilCoolingWater.hpp"
#include "../../model/CoilCoolingWater_Impl.hpp"
#include "../../model/ControllerWaterCoil.hpp"
#include "../../model/ControllerWaterCoil_Impl.hpp"

#include "../../model/AirLoopHVAC.hpp"
#include "../../model/FanVariableVolume.hpp"
#include "../../model/FanVariableVolume_Impl.hpp"
#include "../../model/FanConstantVolume.hpp"
#include "../../model/FanConstantVolume_Impl.hpp"
#include "../../model/FanOnOff.hpp"
#include "../../model/FanOnOff_Impl.hpp"
#include "../../model/FanSystemModel.hpp"
#include "../../model/FanSystemModel_Impl.hpp"
#include "../../model/AirLoopHVACOutdoorAirSystem.hpp"
#include "../../model/AirLoopHVACOutdoorAirSystem_Impl.hpp"

#include <utilities/idd/CoilSystem_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/Coil_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/Controller_WaterCoil_FieldEnums.hxx>
#include <utilities/idd/SetpointManager_MixedAir_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

namespace openstudio {

namespace energyplus {

  boost::optional<IdfObject> ForwardTranslator::translateCoilSystemCoolingWater(model::CoilSystemCoolingWater& modelObject) {

    IdfObject idfObject = createRegisterAndNameIdfObject(openstudio::IddObjectType::CoilSystem_Cooling_Water, modelObject);

    std::string airInletNodeName;
    // InletNodeName
    if (auto mo = modelObject.inletModelObject()) {
      if (auto node = mo->optionalCast<Node>()) {
        airInletNodeName = node->nameString();
      }
    }

    std::string airOutletNodeName;
    // OutletNodeName
    if (auto mo = modelObject.outletModelObject()) {
      if (auto node = mo->optionalCast<Node>()) {
        airOutletNodeName = node->nameString();
      }
    }

    idfObject.setString(CoilSystem_Cooling_WaterFields::AirInletNodeName, airInletNodeName);
    idfObject.setString(CoilSystem_Cooling_WaterFields::AirOutletNodeName, airOutletNodeName);

    // CoolingCoilObjectType
    // CoolingCoilName
    {
      auto coolingCoil = modelObject.coolingCoil();
      if (auto idf = translateAndMapModelObject(coolingCoil)) {
        idfObject.setString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType, idf->iddObject().name());
        idfObject.setString(CoilSystem_Cooling_WaterFields::CoolingCoilName, idf->nameString());
        if (idf->iddObject().type() == IddObjectType::Coil_Cooling_Water) {
          idf->setString(Coil_Cooling_WaterFields::AirInletNodeName, airInletNodeName);
          idf->setString(Coil_Cooling_WaterFields::AirOutletNodeName, airOutletNodeName);
          // Add IddObjectType::Coil_Cooling_Water_DetailedGeometry if implemented
        } else {
          // Shouldn't happen, accepts only Coil:Cooling:Water CoilSystem:Cooling:Water:HeatExchangerAssisted (or Coil:Cooling:Water:DetailedGeometry, not wrapped)
          LOG_AND_THROW(modelObject.briefDescription()
                        << " appears to have a cooling coil that shouldn't have been accepted: " << coolingCoil.briefDescription());
        }
      }

      // TODO: is that true?!
      // Need a SPM:MixedAir on the outlet node (that we **created** just above in IDF directly, so it won't get picked up by the
      // ForwardTranslateAirLoopHVAC method)
      if (boost::optional<AirLoopHVAC> airLoop_ = modelObject.airLoopHVAC()) {
        std::vector<StraightComponent> fans;
        std::vector<ModelObject> supplyComponents = airLoop_->supplyComponents();

        for (auto& supplyComponent : supplyComponents) {
          if (auto fan_ = supplyComponent.optionalCast<FanVariableVolume>()) {
            fans.insert(fans.begin(), std::move(*fan_));
          } else if (auto fan_ = supplyComponent.optionalCast<FanConstantVolume>()) {
            fans.insert(fans.begin(), std::move(*fan_));
          } else if (auto fan_ = supplyComponent.optionalCast<FanSystemModel>()) {
            fans.insert(fans.begin(), std::move(*fan_));
          } else if (auto fan_ = supplyComponent.optionalCast<FanOnOff>()) {
            fans.insert(fans.begin(), std::move(*fan_));
          }
        }

        if (!fans.empty()) {
          // Fan closest to the supply outlet node
          StraightComponent fan = fans.front();
          OptionalNode inletNode = fan.inletModelObject()->optionalCast<Node>();
          OptionalNode outletNode = fan.outletModelObject()->optionalCast<Node>();

          // No reason this wouldn't be ok at this point really...
          // TODO: change to OS_ASSERT?
          if (inletNode && outletNode) {

            IdfObject idf_spm(IddObjectType::SetpointManager_MixedAir);
            idf_spm.setString(SetpointManager_MixedAirFields::Name, airOutletNodeName + " OS Default SPM");
            idf_spm.setString(SetpointManager_MixedAirFields::ControlVariable, "Temperature");

            Node supplyOutletNode = airLoop_->supplyOutletNode();
            idf_spm.setString(SetpointManager_MixedAirFields::ReferenceSetpointNodeName, supplyOutletNode.nameString());

            idf_spm.setString(SetpointManager_MixedAirFields::FanInletNodeName, inletNode->nameString());
            idf_spm.setString(SetpointManager_MixedAirFields::FanOutletNodeName, outletNode->nameString());

            idf_spm.setString(SetpointManager_MixedAirFields::SetpointNodeorNodeListName, airOutletNodeName);

            m_idfObjects.push_back(idf_spm);
          }
        }
      }
    }

    // Availability Schedule Name: Optional Object
    if (boost::optional<Schedule> availabilitySchedule_ = modelObject.availabilitySchedule()) {
      if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(availabilitySchedule_.get())) {
        idfObject.setString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName, wo_->nameString());
      }
    }

    // Dehumidification Control Type: Optional String
    const std::string dehumidificationControlType = modelObject.dehumidificationControlType();
    idfObject.setString(CoilSystem_Cooling_WaterFields::DehumidificationControlType, dehumidificationControlType);

    // Run on Sensible Load: Optional Boolean
    if (modelObject.runonSensibleLoad()) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad, "Yes");
    } else {
      idfObject.setString(CoilSystem_Cooling_WaterFields::RunonSensibleLoad, "No");
    }

    // Run on Latent Load: Optional Boolean
    if (modelObject.runonLatentLoad()) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::RunonLatentLoad, "Yes");
    } else {
      idfObject.setString(CoilSystem_Cooling_WaterFields::RunonLatentLoad, "No");
    }

    // Minimum Air To Water Temperature Offset: Optional Double
    const double minimumAirToWaterTemperatureOffset = modelObject.minimumAirToWaterTemperatureOffset();
    idfObject.setDouble(CoilSystem_Cooling_WaterFields::MinimumAirToWaterTemperatureOffset, minimumAirToWaterTemperatureOffset);

    // Economizer Lockout: Optional Boolean
    if (modelObject.economizerLockout()) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::EconomizerLockout, "Yes");
    } else {
      idfObject.setString(CoilSystem_Cooling_WaterFields::EconomizerLockout, "No");
    }

    // Minimum Water Loop Temperature For Heat Recovery: Optional Double
    const double minimumWaterLoopTemperatureForHeatRecovery = modelObject.minimumWaterLoopTemperatureForHeatRecovery();
    idfObject.setDouble(CoilSystem_Cooling_WaterFields::MinimumWaterLoopTemperatureForHeatRecovery, minimumWaterLoopTemperatureForHeatRecovery);

    // Companion Coil Used For Heat Recovery: Optional Object
    if (boost::optional<HVACComponent> companionCoilUsedForHeatRecovery_ = modelObject.companionCoilUsedForHeatRecovery()) {
      if (auto comp_ = companionCoilUsedForHeatRecovery_->optionalCast<CoilCoolingWater>()) {
        if (comp_->airInletModelObject()) {
          if (boost::optional<IdfObject> idf_ = translateAndMapModelObject(companionCoilUsedForHeatRecovery_.get())) {
            idfObject.setString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, idf_->nameString());
          }
        } else {
          LOG(Warn, modelObject.briefDescription()
                      << " has a companion coil used for heat recovery that is not connected to an AirLoopHVAC / AirLoopHVACOutdoorAirSystem"
                      << " and will not be translated:" << comp_->briefDescription());
        }
      } else {
        // Shouldn't happen, accepts only Coil:Cooling:Water
        LOG_AND_THROW(modelObject.briefDescription() << " appears to have a companion coil used for heat recovery that shouldn't have been accepted: "
                                                     << companionCoilUsedForHeatRecovery_.get().briefDescription());
      }
    }

    return idfObject;
  }  // End of translate function

}  // end namespace energyplus
}  // end namespace openstudio
