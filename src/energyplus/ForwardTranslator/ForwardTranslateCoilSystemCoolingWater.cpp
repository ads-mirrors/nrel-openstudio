/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) Alliance for Sustainable Energy, LLC.
*  See also https://openstudio.net/license
***********************************************************************************************************************/

#include "../ForwardTranslator.hpp"
#include "../../model/Model.hpp"

#include "../../model/CoilSystemCoolingWater.hpp"

// TODO: Check the following class names against object getters and setters.
#include "../../model/Node.hpp"
#include "../../model/Node_Impl.hpp"

#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"

#include "../../model/CoolingCoilsWater.hpp"
#include "../../model/CoolingCoilsWater_Impl.hpp"

#include <utilities/idd/CoilSystem_Cooling_Water_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

namespace openstudio {

namespace energyplus {

  boost::optional<IdfObject> ForwardTranslator::translateCoilSystemCoolingWater(model::CoilSystemCoolingWater& modelObject) {

    // Instantiate an IdfObject of the class to store the values
    IdfObject idfObject = createRegisterAndNameIdfObject(openstudio::IddObjectType::CoilSystem_Cooling_Water, modelObject);
    // If it doesn't have a name, or if you aren't sure you are going to want to return it
    // IdfObject idfObject(openstudio::IddObjectType::CoilSystem_Cooling_Water);
    // m_idfObjects.push_back(idfObject);

    // TODO: Note JM 2018-10-17
    // You are responsible for implementing any additional logic based on choice fields, etc.
    // The ForwardTranslator generator script is meant to facilitate your work, not get you 100% of the way

    // TODO: If you keep createRegisterAndNameIdfObject above, you don't need this.
    // But in some cases, you'll want to handle failure without pushing to the map
    // Name
    idfObject.setName(modelObject.nameString());

    // Air Inlet Node Name: Required Node
    Node airInletNodeName = modelObject.airInletNodeName();
    if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(airInletNodeName)) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::AirInletNodeName, wo_->nameString());
    }

    // Air Outlet Node Name: Required Node
    Node airOutletNodeName = modelObject.airOutletNodeName();
    if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(airOutletNodeName)) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::AirOutletNodeName, wo_->nameString());
    }

    // Availability Schedule Name: Optional Object
    if (boost::optional<Schedule> availabilitySchedule_ = modelObject.availabilitySchedule()) {
      if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(availabilitySchedule_.get())) {
        idfObject.setString(CoilSystem_Cooling_WaterFields::AvailabilityScheduleName, wo_->nameString());
      }
    }

    // Cooling Coil Object Type: Required String
    const std::string coolingCoilObjectType = modelObject.coolingCoilObjectType();
    idfObject.setString(CoilSystem_Cooling_WaterFields::CoolingCoilObjectType, coolingCoilObjectType);

    // Cooling Coil Name: Required Object
    CoolingCoilsWater coolingCoil = modelObject.coolingCoil();
    if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(coolingCoil)) {
      idfObject.setString(CoilSystem_Cooling_WaterFields::CoolingCoilName, wo_->nameString());
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
    if (boost::optional<CoolingCoilsWater> companionCoilUsedForHeatRecovery_ = modelObject.companionCoilUsedForHeatRecovery()) {
      if (boost::optional<IdfObject> wo_ = translateAndMapModelObject(companionCoilUsedForHeatRecovery_.get())) {
        idfObject.setString(CoilSystem_Cooling_WaterFields::CompanionCoilUsedForHeatRecovery, wo_->nameString());
      }
    }

    return idfObject;
  }  // End of translate function

}  // end namespace energyplus
}  // end namespace openstudio
