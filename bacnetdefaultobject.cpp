#include "bacnetdefaultobject.h"

#include "bacnetprimitivedata.h"
#include "bacnetconstructeddata.h"
#include "bacnetdataabstract.h"

using namespace Bacnet;

BacnetDefaultObject *BacnetDefaultObject::_instance = 0;

BacnetDefaultObject::BacnetDefaultObject()
{
}

BacnetDefaultObject *BacnetDefaultObject::instance()
{
    if (_instance == 0)
        _instance = new BacnetDefaultObject();
    return _instance;
}

Bacnet::BacnetDataInterface *BacnetDefaultObject::getProperty(BacnetObjectType::ObjectType objType, BacnetProperty::Identifier propId) {
    QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> &objDefaultProperties = _properties[objType];
    if (objDefaultProperties.isEmpty()) {
        return 0;
    }
    return objDefaultProperties.value(propId);
}

const QMap<BacnetProperty::Identifier, Bacnet::BacnetDataInterface*> &BacnetDefaultObject::defaultProperties(BacnetObjectType::ObjectType objType)
{
    return _properties[objType];
}

Bacnet::BacnetDataInterface *BacnetDefaultObject::createDataForObjectProperty(BacnetObjectType::ObjectType type, BacnetProperty::Identifier propertyId, quint32 arrayIndex)
{
    switch (type)
    {
    case (BacnetObjectType::Device):
        switch (propertyId)
        {
        case (BacnetProperty::ObjectIdentifier):                return new Bacnet::ObjectIdentifier();
        case (BacnetProperty::ObjectName):                      return new Bacnet::CharacterString();
        case (BacnetProperty::ObjectType):                      return new Bacnet::UnsignedInteger();
        case (BacnetProperty::SystemStatus):                    return new Bacnet::DeviceStatus();
        case (BacnetProperty::VendorName):                      return new Bacnet::CharacterString();
        case (BacnetProperty::VendorIdentifier):                return new Bacnet::Unsigned16();//DataType::Unsigned16;
        case (BacnetProperty::ModelName):                       return new Bacnet::CharacterString();
        case (BacnetProperty::FirmwareRevision):                return new Bacnet::CharacterString();
        case (BacnetProperty::ApplicationSoftwareVersion):      return new Bacnet::CharacterString();
        case (BacnetProperty::Location):                        return new Bacnet::CharacterString();
        case (BacnetProperty::Description):                     return new Bacnet::CharacterString();
        case (BacnetProperty::ProtocolVersion):                 return new Bacnet::UnsignedInteger();
        case (BacnetProperty::ProtocolRevision):                return new Bacnet::UnsignedInteger();
        case (BacnetProperty::ProtocolServicesSupported):       return new Bacnet::ServicesSupported();
        case (BacnetProperty::ProtocolObjectTypesSupported):    return new Bacnet::ObjectTypesSupported();
        case (BacnetProperty::ObjectList):                      return new Bacnet::BacnetArray(DataType::BACnetObjectIdentifier);
        case (BacnetProperty::MaxApduLengthAccepted):           return new Bacnet::UnsignedInteger();
        case (BacnetProperty::SegmentationSupported):           return new Bacnet::Segmentation();
            //        case (BacnetProperty::MaxSegmentsAccepted):             return DataType::Unsigned;
            //        case (BacnetProperty::VtClassesSupported):              return DataType::BACnetList | DataType::BACnetVTClass;;
            //        case (BacnetProperty::ActiveVtSessions):                return DataType::BACnetList | DataType::BACnetVTSession;
            //        case (BacnetProperty::LocalTime):                       return DataType::Time;
            //        case (BacnetProperty::LocalDate):                       return DataType::Date;
            //        case (BacnetProperty::UtcOffset):                       return DataType::Signed;
            //        case (BacnetProperty::DaylightSavingsStatus):           return DataType::BOOLEAN;
            //        case (BacnetProperty::ApduSegmentTimeout):              return DataType::Unsigned;
        case (BacnetProperty::ApduTimeout):                     return new Bacnet::UnsignedInteger();
        case (BacnetProperty::NumberOfAPDURetries):             return new Bacnet::UnsignedInteger();
            //        case (BacnetProperty::ListOfSessionKeys):               return DataType::BACnetList | DataType::BACnetSessionKey;
            //        case (BacnetProperty::TimeSynchronizationRecipients):   return DataType::BACnetList | DataType::BACnetRecipient;
            //        case (BacnetProperty::MaxMaster):                       return DataType::Unsigned;
            //        case (BacnetProperty::MaxInfoFrames):                   return DataType::Unsigned;
        case (BacnetProperty::DeviceAddressBinding):            return new Bacnet::BacnetList(DataType::BACnetAddressBinding);
        case (BacnetProperty::DatabaseRevision):                return new Bacnet::UnsignedInteger();
            //        case (BacnetProperty::ConfigurationFiles):              return DataType::BACnetArray | DataType::BACnetObjectIdentifier;
            //        case (BacnetProperty::LastRestoreTime):                 return DataType::BACnetTimeStamp;
            //        case (BacnetProperty::BackupFailureTimeout):            return DataType::Unsigned16;
        case (BacnetProperty::ActiveCovSubscriptions):          return new Bacnet::BacnetList(DataType::BACnetCOVSubscription);
            //        case (BacnetProperty::SlaveProxyEnable):                return DataType::BACnetArray | DataType::BOOLEAN;
            //        case (BacnetProperty::ManualSlaveAddressBinding):       return DataType::BACnetList | DataType::BACnetAddressBinding;
            //        case (BacnetProperty::AutoSlaveDiscovery):              return DataType::BACnetArray | DataType::BOOLEAN;
            //        case (BacnetProperty::SlaveAddressBinding):             return DataType::BACnetList | DataType::BACnetAddressBinding;
            //        case (BacnetProperty::ProfileName):                     return DataType::CharacterString;
        default:
            return new Bacnet::DataAbstract();
        }
        break;
    case (BacnetObjectType::AnalogInput):
        switch (propertyId)
        {
        case (BacnetProperty::ObjectIdentifier):                return new Bacnet::ObjectIdentifier();
        case (BacnetProperty::ObjectName):                      return new Bacnet::CharacterString();
        case (BacnetProperty::ObjectType):                      return new Bacnet::UnsignedInteger();
        case (BacnetProperty::PresentValue):                    return new Bacnet::Real();
        case (BacnetProperty::Description):                     return new Bacnet::CharacterString();
        case (BacnetProperty::DeviceType):                      return new Bacnet::CharacterString();
        case (BacnetProperty::StatusFlags):                     return new Bacnet::BitString();
        case (BacnetProperty::EventState):                      return new Bacnet::Enumerated();
        case (BacnetProperty::Reliability):                     return new Bacnet::Enumerated();
        case (BacnetProperty::OutOfService):                    return new Bacnet::Boolean();
        case (BacnetProperty::UpdateInterval):                  return new Bacnet::UnsignedInteger();
        case (BacnetProperty::Units):                           return new Bacnet::Enumerated();
        case (BacnetProperty::MinPresValue):                    return new Bacnet::Real();
        case (BacnetProperty::MaxPresValue):                    return new Bacnet::Real();
        case (BacnetProperty::Resolution):                      return new Bacnet::Real();
        case (BacnetProperty::CovIncrement):                    return new Bacnet::Real();
        case (BacnetProperty::TimeDelay):                       return new Bacnet::UnsignedInteger();
        case (BacnetProperty::NotificationClass):               return new Bacnet::UnsignedInteger();
        case (BacnetProperty::HighLimit):                       return new Bacnet::Real();
        case (BacnetProperty::LowLimit):                        return new Bacnet::Real();
        case (BacnetProperty::Deadband):                        return new Bacnet::Real();
        case (BacnetProperty::LimitEnable):                     return new Bacnet::BitString();
        case (BacnetProperty::EventEnable):                     return new Bacnet::BitString();
        case (BacnetProperty::AckedTransitions):                return new Bacnet::BitString();
        case (BacnetProperty::NotifyType):                      return new Bacnet::Enumerated();
        case (BacnetProperty::EventTimeStamps):                 return new Bacnet::BacnetArray(/*Bacnet::TimeStamp*/);
        case (BacnetProperty::ProfileName):                     return new Bacnet::CharacterString();
        default:
            return new Bacnet::DataAbstract();
        }
        break;
    case (BacnetObjectType::AnalogOutput):
        switch (propertyId)
        {
        case (BacnetProperty::ObjectIdentifier):                return new Bacnet::ObjectIdentifier();
        case (BacnetProperty::ObjectName):                      return new Bacnet::CharacterString();
        case (BacnetProperty::ObjectType):                      return new Bacnet::UnsignedInteger();
        case (BacnetProperty::PresentValue):                    return new Bacnet::Real();
        case (BacnetProperty::Description):                     return new Bacnet::CharacterString();
        case (BacnetProperty::DeviceType):                      return new Bacnet::CharacterString();
        case (BacnetProperty::StatusFlags):                     return new Bacnet::BitString();
        case (BacnetProperty::EventState):                      return new Bacnet::Enumerated();
        case (BacnetProperty::Reliability):                     return new Bacnet::Enumerated();
        case (BacnetProperty::OutOfService):                    return new Bacnet::Boolean();
        case (BacnetProperty::Units):                           return new Bacnet::Enumerated();
        case (BacnetProperty::MinPresValue):                    return new Bacnet::Real();
        case (BacnetProperty::MaxPresValue):                    return new Bacnet::Real();
        case (BacnetProperty::Resolution):                      return new Bacnet::Real();
        case (BacnetProperty::RelinquishDefault):               return new Bacnet::Real();
            //        case (BacnetProperty::PriorityArray):                   return new Bacnet::PRIORITYARRAY
        case (BacnetProperty::CovIncrement):                    return new Bacnet::Real();
        case (BacnetProperty::TimeDelay):                       return new Bacnet::UnsignedInteger();
        case (BacnetProperty::NotificationClass):               return new Bacnet::UnsignedInteger();
        case (BacnetProperty::HighLimit):                       return new Bacnet::Real();
        case (BacnetProperty::LowLimit):                        return new Bacnet::Real();
        case (BacnetProperty::Deadband):                        return new Bacnet::Real();
        case (BacnetProperty::LimitEnable):                     return new Bacnet::BitString();
        case (BacnetProperty::EventEnable):                     return new Bacnet::BitString();
        case (BacnetProperty::AckedTransitions):                return new Bacnet::BitString();
        case (BacnetProperty::NotifyType):                      return new Bacnet::Enumerated();
            //        case (BacnetProperty::EventTimeStamps):                 return new Bacnet::BacnetArray(/*Bacnet::TimeStamp*/);
        case (BacnetProperty::ProfileName):                     return new Bacnet::CharacterString();
        default:
            return new Bacnet::DataAbstract();
        }
        break;
    case (BacnetObjectType::BinaryInput):
        switch (propertyId)
        {
        case (BacnetProperty::ObjectIdentifier):                return new Bacnet::ObjectIdentifier();
        case (BacnetProperty::ObjectName):                      return new Bacnet::CharacterString();
        case (BacnetProperty::ObjectType):                      return new Bacnet::UnsignedInteger();
        case (BacnetProperty::PresentValue):                    return new Bacnet::Enumerated();
        case (BacnetProperty::Description):                     return new Bacnet::CharacterString();
        case (BacnetProperty::DeviceType):                      return new Bacnet::CharacterString();
        case (BacnetProperty::StatusFlags):                     return new Bacnet::BitString();
        case (BacnetProperty::EventState):                      return new Bacnet::Enumerated();
        case (BacnetProperty::Reliability):                     return new Bacnet::Enumerated();
        case (BacnetProperty::OutOfService):                    return new Bacnet::Boolean();
        case (BacnetProperty::Polarity):                        return new Bacnet::Enumerated();
        case (BacnetProperty::InactiveText):                    return new Bacnet::CharacterString();
        case (BacnetProperty::ActiveText):                      return new Bacnet::CharacterString();
        case (BacnetProperty::ChangeOfStateTime):               return new Bacnet::DateTime();
        case (BacnetProperty::ChangeOfStateCount):              return new Bacnet::UnsignedInteger();
        case (BacnetProperty::TimeOfStateCountReset):           return new Bacnet::DateTime();
        case (BacnetProperty::ElapsedActiveTime):               return new Bacnet::UnsignedInteger();
        case (BacnetProperty::TimeOfActiveTimeReset):           return new Bacnet::DateTime();
        case (BacnetProperty::TimeDelay):                       return new Bacnet::UnsignedInteger();
        case (BacnetProperty::NotificationClass):               return new Bacnet::UnsignedInteger();
        case (BacnetProperty::AlarmValue):                      return new Bacnet::Enumerated();
        case (BacnetProperty::EventEnable):                     return new Bacnet::BitString();
        case (BacnetProperty::AckedTransitions):                return new Bacnet::BitString();
        case (BacnetProperty::NotifyType):                      return new Bacnet::Enumerated();
        case (BacnetProperty::EventTimeStamps):                 return new Bacnet::BacnetArray(/*Bacnet::TimeStamp*/);
        case (BacnetProperty::ProfileName):                     return new Bacnet::CharacterString();
        default:
            return new Bacnet::DataAbstract();
        }
        break;
    case (BacnetObjectType::BinaryOutput):
        switch (propertyId)
        {
        default:
            return new Bacnet::DataAbstract();
        }
        break;
   case (BacnetObjectType::AnalogValue):
        switch(propertyId)
        {
        case (BacnetProperty::PresentValue):                return new Bacnet::Real();
        default:
            return new Bacnet::DataAbstract();
        }
    default:
        return 0;
    }
}

quint32 BacnetDefaultObject::proeprtyType(BacnetObjectType::ObjectType type, BacnetProperty::Identifier propertyId)
{
    switch (type)
    {
    case (BacnetObjectType::Device):
        switch (propertyId)
        {
        case (BacnetProperty::ObjectIdentifier):                return DataType::BACnetObjectIdentifier;
        case (BacnetProperty::ObjectName):                      return DataType::CharacterString;
        case (BacnetProperty::ObjectType):                      return DataType::BACnetObjectType;
        case (BacnetProperty::SystemStatus):                    return DataType::BACnetDeviceStatus;
        case (BacnetProperty::VendorName):                      return DataType::CharacterString;
        case (BacnetProperty::VendorIdentifier):                return DataType::Unsigned16;
        case (BacnetProperty::ModelName):                       return DataType::CharacterString;
        case (BacnetProperty::FirmwareRevision):                return DataType::CharacterString;
        case (BacnetProperty::ApplicationSoftwareVersion):      return DataType::CharacterString;
        case (BacnetProperty::Location):                        return DataType::CharacterString;
        case (BacnetProperty::Description):                     return DataType::CharacterString;
        case (BacnetProperty::ProtocolVersion):                 return DataType::Unsigned;
        case (BacnetProperty::ProtocolRevision):                return DataType::Unsigned;
        case (BacnetProperty::ProtocolServicesSupported):       return DataType::BACnetServicesSupported;
        case (BacnetProperty::ProtocolObjectTypesSupported):    return DataType::BACnetObjectTypesSupported;
        case (BacnetProperty::ObjectList):                      return DataType::BACnetArray | DataType::BACnetObjectIdentifier;;
        case (BacnetProperty::MaxApduLengthAccepted):           return DataType::Unsigned;
        case (BacnetProperty::SegmentationSupported):           return DataType::BACnetSegmentation;;
            //        case (BacnetProperty::MaxSegmentsAccepted):             return DataType::Unsigned;
            //        case (BacnetProperty::VtClassesSupported):              return DataType::BACnetList | DataType::BACnetVTClass;;
            //        case (BacnetProperty::ActiveVtSessions):                return DataType::BACnetList | DataType::BACnetVTSession;
            //        case (BacnetProperty::LocalTime):                       return DataType::Time;
            //        case (BacnetProperty::LocalDate):                       return DataType::Date;
            //        case (BacnetProperty::UtcOffset):                       return DataType::Signed;
            //        case (BacnetProperty::DaylightSavingsStatus):           return DataType::BOOLEAN;
            //        case (BacnetProperty::ApduSegmentTimeout):              return DataType::Unsigned;
        case (BacnetProperty::ApduTimeout):                     return DataType::Unsigned;
        case (BacnetProperty::NumberOfAPDURetries):             return DataType::Unsigned;
            //        case (BacnetProperty::ListOfSessionKeys):               return DataType::BACnetList | DataType::BACnetSessionKey;
            //        case (BacnetProperty::TimeSynchronizationRecipients):   return DataType::BACnetList | DataType::BACnetRecipient;
            //        case (BacnetProperty::MaxMaster):                       return DataType::Unsigned;
            //        case (BacnetProperty::MaxInfoFrames):                   return DataType::Unsigned;
        case (BacnetProperty::DeviceAddressBinding):            return DataType::BACnetList | DataType::BACnetAddressBinding;
        case (BacnetProperty::DatabaseRevision):                return DataType::Unsigned;
            //        case (BacnetProperty::ConfigurationFiles):              return DataType::BACnetArray | DataType::BACnetObjectIdentifier;
            //        case (BacnetProperty::LastRestoreTime):                 return DataType::BACnetTimeStamp;
            //        case (BacnetProperty::BackupFailureTimeout):            return DataType::Unsigned16;
        case (BacnetProperty::ActiveCovSubscriptions):          return DataType::BACnetList | DataType::BACnetCOVSubscription;
            //        case (BacnetProperty::SlaveProxyEnable):                return DataType::BACnetArray | DataType::BOOLEAN;
            //        case (BacnetProperty::ManualSlaveAddressBinding):       return DataType::BACnetList | DataType::BACnetAddressBinding;
            //        case (BacnetProperty::AutoSlaveDiscovery):              return DataType::BACnetArray | DataType::BOOLEAN;
            //        case (BacnetProperty::SlaveAddressBinding):             return DataType::BACnetList | DataType::BACnetAddressBinding;
            //        case (BacnetProperty::ProfileName):                     return DataType::CharacterString;
        default:
            return DataType::InvalidType;
        }
        break;
    case (BacnetObjectType::AnalogInput):
        switch (propertyId)
        {
        default: return DataType::InvalidType;
        }
        break;
    case (BacnetObjectType::AnalogOutput):
        switch (propertyId)
        {
        default: return DataType::InvalidType;
        }
        break;
    case (BacnetObjectType::BinaryInput):
        switch (propertyId)
        {
        default: return DataType::InvalidType;
        }
        break;
    case (BacnetObjectType::BinaryOutput):
        switch (propertyId)
        {
        default: return DataType::InvalidType;
        }
        break;
    default: return DataType::InvalidType;
    }
}

Bacnet::BacnetDataInterface *BacnetDefaultObject::createDataType(DataType::DataType type)
{
    switch (type)
    {
    case (DataType::Null):                  return new Bacnet::Null();
    case (DataType::BOOLEAN):               return new Bacnet::Boolean();
    case (DataType::Unsigned):              return new Bacnet::UnsignedInteger();
    case (DataType::Signed):                return new Bacnet::SignedInteger();
    case (DataType::Real):                  return new Bacnet::Real();
    case (DataType::Double):                return new Bacnet::Double();
    case (DataType::OctetString):           return new Bacnet::OctetString();
    case (DataType::CharacterString):       return new Bacnet::CharacterString();
    case (DataType::BitString):             return new Bacnet::BitString();
    case (DataType::Enumerated):            return new Bacnet::Enumerated();
    case (DataType::Date):                  return new Bacnet::Date();
    case (DataType::Time):                  return new Bacnet::Time();
    case (DataType::BACnetObjectIdentifier):return new Bacnet::ObjectIdentifier();
    default:
        return new Bacnet::DataAbstract();
    }
}
