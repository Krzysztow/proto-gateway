#ifndef BACNETCOMMON_H
#define BACNETCOMMON_H

#include <stdio.h>
#include <QtCore>

/**
  This header is meant to provide some common data for all BACnet classes within the project.
  */

namespace BacnetObjectType {
    enum ObjectType {
        Accumulator 	= 23,
        AnalogInput 	= 0,
        AnalogOutput 	= 1,
        AnalogValue 	= 2,
        Averaging 	= 18,
        BinaryInput 	= 3,
        BinaryOutput 	= 4,
        BinaryValue 	= 5,
        Calendar 	= 6,
        Command 	= 7,
        Device 	= 8,
        EventEnrollment 	= 9,
        File 	= 10,
        Group 	= 11,
        LifeSafetyPoint 	= 21,
        LifeSafetyZone 	= 22,
        Loop 	= 12,
        MultiStateInput 	= 13,
        MultiStateOutput 	= 14,
        MultiStateValue 	= 19,
        NotificationClass 	= 15,
        Program 	= 16,
        PulseConverter 	= 24,
        Schedule 	= 17,
        // see averaging 	= 18,
        // see multiStateValue 	= 19,
        TrendLog 	= 20,
        // see lifeSafetyPoint 	= 21,
        // see lifeSafetyZone 	= 22,
        // see accumulator 	= 23,
        // see pulseConverter 	= 24
        MaximumEnumType = PulseConverter,
        Undefined = 255//not defiend by standard, but we have space for it - only 10 bits are used
                };
}

namespace BacnetAbort {
    enum AbortReason {
        ReasonOther                            = 0,
        ReasonBufferOverflow                   = 1,
        ReasonInvalidApduInThisState           = 2,
        ReasonPreemptedByHigherPriorityTask    = 3,
        ReasonSegmentationNotSupported          = 4
                                              };
}

namespace BacnetReject {
    enum RejectReason {
        ReasonOther                     = 0,
        ReasonBufferOverflow            = 1,
        ReasonInconsistentParameters    = 2,
        ReasonInvalidParameterDataType 	= 3,
        ReasonInvalidTag                = 4,
        ReasonMissingRequiredParameter 	= 5,
        ReasonParameterOutOfRange 	= 6,
        ReasonTooManyArguments          = 7,
        ReasonUndefinedEnumeration 	= 8,
        ReasonUnrecognizedService 	= 9,

        ReasonNotRejected//used internally, not specified by specificiation
    };
}

namespace BacnetError {
    enum ErrorClass {
        ClassDevice 	= 0,
        ClassObject 	= 1,
        ClassProperty 	= 2,
        ClassResources 	= 3,
        ClassSecurity 	= 4,
        ClassServices 	= 5,
        ClassVt 	= 6,

        ClassNoError
    };

    enum ErrorCode {
        CodeOther                                   	= 0,
        CodeAuthenticationFailed                   	= 1,
        CodeCharacterSetNotSupported             	= 41,
        CodeConfigurationInProgress               	= 2,
        CodeDatatypeNotSupported                  	= 47,
        CodeDeviceBusy                             	= 3,
        CodeDuplicateName                          	= 48,
        CodeDplicateObjectId                      	= 49,
        CodeDynamicCreationNotSupported          	= 4,
        CodeFileAccessDenied                      	= 5,
        CodeIncompatibleSecurityLevels            	= 6,
        CodeInconsistentParameters                 	= 7,
        CodeInconsistentSelectionCriterion        	= 8,
        CodeInvalidArrayIndex                     	= 42,
        CodeInvalidConfigurationData              	= 46,
        CodeInvalidDataType                       	= 9,
        CodeInvalidFileAccessMethod              	= 10,
        CodeInvalidFileStartPosition             	= 11,
        CodeInvalidOperatorName                   	= 12,
        CodeInvalidParameterDataType             	= 13,
        CodeInvalidTimeStamp                      	= 14,
        CodeKeyGenerationError                    	= 15,
        CodeMissingRequiredParameter              	= 16,
        CodeNoObjectsOfSpecifiedType            	= 17,
        CodeNoSpaceForObject                     	= 18,
        CodeNoSpaceToAddListElement                     = 19,
        CodeNoSpaceToWriteProperty              	= 20,
        CodeNoVtSessionsAvailable                	= 21,
        CodeObjectDeletionNotPermitted           	= 23,
        CodeObjectIdentifierAlreadyExists        	= 24,
        CodeOperationalProblem                     	= 25,
        CodeOptionalFunctionalityNotSupported   	= 45,
        CodePasswordFailure                        	= 26,
        CodePropertyIsNotAList                  	= 22,
        CodePropertyIsNotAnArray                	= 50,
        CodeReadAccessDenied                      	= 27,
        CodeSecurityNotSupported                  	= 28,
        CodeServiceRequestDenied                  	= 29,
        CodeTimeout                                 	= 30,
        CodeUnknownObject                               = 31,
        CodeUnknownProperty                             = 32,
        // this enumeration was removed                 	= 33,
        CodeUnknownVtClass                              = 34,
        CodeUnknownVtSession                            = 35,
        CodeUnsupportedObjectType                       = 36,
        CodeValueOutOfRange                             = 37,
        CodeVtSessionAlreadyClosed                      = 38,
        CodeVtSessionTerminationFailure                 = 39,
        CodeWriteAccessDenied                           = 40,
        // see characterSetNotSupported              	= 41,
        // see invalidArrayIndex                      	= 42,
        CodeCovSubscriptionFailed                       = 43,
        CodeNotCovProperty                              = 44,
        // see optionalFunctionalityNotSupported,    	= 45,
        // see invalidConfigurationData               	= 46,
        // see datatypeNotSupported                   	= 47,
        // see duplicateName                           	= 48,
        // see duplicateObjectId                      	= 49,
        // see propertyIsNotAnArray                 	= 50

        CodeNoError
    };

}

namespace BacnetProperty
{
    enum Identifier {
        AcceptedModes                              	= 175,
        AckedTransitions                           	= 0,
        AckRequired                                	= 1,
        Action                                      	= 2,
        ActionText                                 	= 3,
        ActiveText                                 	= 4,
        ActiveVtSessions                          	= 5,
        ActiveCovSubscriptions                    	= 152,
        AdjustValue                                	= 176,
        AlarmValue                                 	= 6,
        AlarmValues                                	= 7,
        All                                         	= 8,
        AllWritesSuccessful                       	= 9,
        ApduSegmentTimeout                        	= 10,
        ApduTimeout                                	= 11,
        ApplicationSoftwareVersion                	= 12,
        Archive                                     	= 13,
        AttemptedSamples                           	= 124,
        AutoSlaveDiscovery                        	= 169,
        AverageValue                               	= 125,
        BackupFailureTimeout                      	= 153,
        Bias                                        	= 14,
        BufferSize                                 	= 126,
        ChangeOfStateCount                       	= 15,
        ChangeOfStateTime                        	= 16,
        // see notificationClass                   	= 17,
        // the property in this place was deleted   	= 18,
        ClientCovIncrement                        	= 127,
        ConfigurationFiles                         	= 154,
        ControlledVariableReference               	= 19,
        ControlledVariableUnits                   	= 20,
        ControlledVariableValue                   	= 21,
        Count                                       	= 177,
        CountBeforeChange                         	= 178,
        CountChangeTime                           	= 179,
        CovIncrement                               	= 22,
        CovPeriod                                  	= 180,
        CovResubscriptionInterval                 	= 128,
        // currentNotifyTime                      	= 129, This property was deleted in version 1 revision 3.
        DatabaseRevision                           	= 155,
        DateList                                   	= 23,
        DaylightSavingsStatus                     	= 24,
        Deadband                                    	= 25,
        DerivativeConstant                         	= 26,
        DerivativeConstantUnits                   	= 27,
        Description                                 	= 28,
        DescriptionOfHalt                         	= 29,
        DeviceAddressBinding                      	= 30,
        DeviceType                                 	= 31,
        DirectReading                              	= 156,
        EffectivePeriod                            	= 32,
        ElapsedActiveTime                         	= 33,
        ErrorLimit                                 	= 34,
        EventEnable                                	= 35,
        EventState                                 	= 36,
        EventTimeStamps                           	= 130,
        EventType                                  	= 37,
        EventParameters                            	= 83, // renamed from previous version
        ExceptionSchedule                          	= 38,
        FaultValues                                	= 39,
        FeedbackValue                              	= 40,
        FileAccessMethod                          	= 41,
        FileSize                                   	= 42,
        FileType                                   	= 43,
        FirmwareRevision                           	= 44,
        HighLimit                                  	= 45,
        InactiveText                               	= 46,
        InProcess                                  	= 47,
        InputReference                             	= 181,
        InstanceOf                                 	= 48,
        IntegralConstant                           	= 49,
        IntegralConstantUnits                     	= 50,
        // issueConfirmedNotifications            	= 51, This property was deleted in version 1 revision 4.
        LastNotifyRecord                          	= 173,
        LastRestoreTime                           	= 157,
        LifeSafetyAlarmValues                    	= 166,
        LimitEnable                                	= 52,
        LimitMonitoringInterval                   	= 182,
        ListOfGroupMembers                       	= 53,
        ListOfObjectPropertyReferences          	= 54,
        ListOfSessionKeys                        	= 55,
        LocalDate                                  	= 56,
        LocalTime                                  	= 57,
        Location                                    	= 58,
        LogBuffer                                  	= 131,
        LogDeviceObjectProperty                  	= 132,
        LogEnable                                  	= 133,
        LogInterval                                	= 134,
        LoggingObject                              	= 183,
        LoggingRecord                              	= 184,
        LowLimit                                   	= 59,
        MaintenanceRequired                        	= 158,
        ManipulatedVariableReference              	= 60,
        ManualSlaveAddressBinding                	= 170,
        MaximumOutput                              	= 61,
        MaximumValue                               	= 135,
        MaximumValueTimestamp                     	= 149,
        MaxApduLengthAccepted                    	= 62,
        MaxInfoFrames                             	= 63,
        MaxMaster                                  	= 64,
        MaxPresValue                              	= 65,
        MaxSegmentsAccepted                       	= 167,
        MemberOf                                   	= 159,
        MinimumOffTime                            	= 66,
        MinimumOnTime                             	= 67,
        MinimumOutput                              	= 68,
        MinimumValue                               	= 136,
        MinimumValueTimestamp                     	= 150,
        MinPresValue                              	= 69,
        Mode                                        	= 160,
        ModelName                                  	= 70,
        ModificationDate                           	= 71,
        NotificationClass                          	= 17, // renamed from previous version
        NotificationThreshold                      	= 137,
        NotifyType                                 	= 72,
        NumberOfAPDURetries                      	= 73,
        NumberOfStates                            	= 74,
        ObjectIdentifier                           	= 75,
        ObjectList                                 	= 76,
        ObjectName                                 	= 77,
        ObjectPropertyReference                   	= 78,
        ObjectType                                 	= 79,
        OperationExpected                          	= 161,
        Optional                                    	= 80,
        OutOfService                              	= 81,
        OutputUnits                                	= 82,
        // see eventParameters                     	= 83,
        Polarity                                    	= 84,
        Prescale                                    	= 185,
        PresentValue                               	= 85,
        // previousNotifyTime                     	= 138, This property was deleted in version 1 revision 3.
        Priority                                    	= 86,
        PulseRate                                  	= 186,
        PriorityArray                              	= 87,
        PriorityForWriting                        	= 88,
        ProcessIdentifier                          	= 89,
        ProfileName                                	= 168,
        ProgramChange                              	= 90,
        ProgramLocation                            	= 91,
        ProgramState                               	= 92,
        ProportionalConstant                       	= 93,
        ProportionalConstantUnits                 	= 94,
        // protocolConformanceClass               	= 95,
        ProtocolObjectTypesSupported             	= 96,
        ProtocolRevision                           	= 139,
        ProtocolServicesSupported                 	= 97,
        ProtocolVersion                            	= 98,
        ReadOnly                                   	= 99,
        ReasonForHalt                             	= 100,
        // recipient                                	= 101,
        RecipientList                              	= 102,
        RecordsSinceNotification                  	= 140,
        RecordCount                                	= 141,
        Reliability                                 	= 103,
        RelinquishDefault                          	= 104,
        Required                                    	= 105,
        Resolution                                  	= 106,
        Scale                                       	= 187,
        ScaleFactor                                	= 188,
        ScheduleDefault                            	= 174,
        SegmentationSupported                      	= 107,
        Setpoint                                    	= 108,
        SetpointReference                          	= 109,
        SlaveAddressBinding                       	= 171,
        Setting                                     	= 162,
        Silenced                                    	= 163,
        StartTime                                  	= 142,
        StateText                                  	= 110,
        StatusFlags                                	= 111,
        StopTime                                   	= 143,
        StopWhenFull                              	= 144,
        SystemStatus                               	= 112,
        TimeDelay                                  	= 113,
        TimeOfActiveTimeReset                   	= 114,
        TimeOfStateCountReset                   	= 115,
        TimeSynchronizationRecipients             	= 116,
        TotalRecordCount                          	= 145,
        TrackingValue                              	= 164,
        Units                                       	= 117,
        UpdateInterval                             	= 118,
        UpdateTime                                 	= 189,
        UtcOffset                                  	= 119,
        ValidSamples                               	= 146,
        ValueBeforeChange                         	= 190,
        ValueSet                                   	= 191,
        ValueChangeTime                           	= 192,
        VarianceValue                              	= 151,
        VendorIdentifier                           	= 120,
        VendorName                                 	= 121,
        VtClassesSupported                        	= 122,
        WeeklySchedule                             	= 123,
        // see attemptedSamples                    	= 124,
        // see averageValue                        	= 125,
        // see bufferSize                          	= 126,
        // see clientCovIncrement                 	= 127,
        // see covResubscriptionInterval          	= 128,
        // unused                                   	= 129,
        // see eventTimeStamps                    	= 130,
        // see logBuffer                           	= 131,
        // see logDeviceObjectProperty           	= 132,
        // see logEnable                           	= 133,
        // see logInterval                         	= 134,
        // see maximumValue                        	= 135,
        // see minimumValue                        	= 136,
        // see notificationThreshold               	= 137,
        // unused                                   	= 138,
        // see protocolRevision                    	= 139,
        // see recordsSinceNotification           	= 140,
        // see recordCount                         	= 141,
        // see startTime                           	= 142,
        // see stopTime                            	= 143,
        // see stopWhenFull                       	= 144,
        // see totalRecordCount                   	= 145,
        // see validSamples                        	= 146,
        WindowInterval                             	= 147,
        WindowSamples                              	= 148,
        ZoneMembers                                	= 165,
        UndefinedProperty                               = 255
                                                          // see maximumValueTimestamp              	= 149,
                                                          // see minimumValueTimestamp              	= 150,
                                                          // see varianceValue                       	= 151,
                                                          // see activeCovSubscriptions             	= 152,
                                                          // see backupFailureTimeout               	= 153,
                                                          // see configurationFiles                  	= 154,
                                                          // see databaseRevision                    	= 155,
                                                          // see directReading                       	= 156,
                                                          // see lastRestoreTime,                   	= 157,
                                                          // see maintenanceRequired                 	= 158,
                                                          // see memberOf                            	= 159,
                                                          // see mode                                 	= 160,
                                                          // see operationExpected                   	= 161,
                                                          // see setting                              	= 162,
                                                          // see silenced                             	= 163,
                                                          // see trackingValue                       	= 164,
                                                          // see zoneMembers                         	= 165,
                                                          // see lifeSafetyAlarmValues             	= 166,
                                                          // see maxSegmentsAccepted                	= 167,
                                                          // see profileName                         	= 168,
                                                          // see autoSlaveDiscovery                 	= 169,
                                                          // see manualSlaveAddressBinding         	= 170,
                                                          // see slaveAddressBinding                	= 171,
                                                          // see slaveProxyEnable                   	= 172,
                                                          // see lastNotifyTime                     	= 173,
                                                          // see scheduleDefault                     	= 174,
                                                          // see acceptedModes                       	= 175,
                                                          // see adjustValue                         	= 176,
                                                          // see count                                	= 177,
                                                          // see countBeforeChange                  	= 178,
                                                          // see countChangeTime                    	= 179,
                                                          // see covPeriod                           	= 180,
                                                          // see inputReference                      	= 181,
                                                          // see limitMonitoringInterval            	= 182,
                                                          // see loggingDevice                       	= 183,
                                                          // see loggingRecord                       	= 184,
                                                          // see prescale                             	= 185,
                                                          // see pulseRate                           	= 186,
                                                          // see scale                                	= 187,
                                                          // see scaleFactor                         	= 188,
                                                          // see updateTime                          	= 189,
                                                          // see valueBeforeChange                  	= 190,
                                                          // see valueSet                            	= 191,
                                                          // see valueChangeTime                    	= 192,
                                                      };
}

namespace Bacnet
{
    enum BacnetAddressSize {
        //this is a size of the address being concatenation of IPv4 address and port number
        BacnetBipAddrSize = 6
                        };

    /* From Clause 6 of BACnet specification, we get limitations to the NPDU size for a layer */
    enum BacnetFrameSize {
        //as specified in standard
        NpduMaxSize = 1497,
        //maximum header size is a sum of all optional and must-be fields
        NpduMaxHeaderSize = 1 /*version*/ + 1/*NPCI*/ + 2/*DNET*/ + 1/*DLEN*/ +
                            BacnetBipAddrSize + 2 /*SNET*/ + 1 /*SLEN*/ +
                            BacnetBipAddrSize + 1 /*hop count*/ + 1 /*msg type*/ +
                            2 /*vendorId*/,
                            BvllMaxHeaderSize = 1 /*microprotocol*/ + 1 /*code*/ + 2 /*lengt*/,
                            //to be on the safe side, subtract maximum NpduHeaderSize
                            ApduMaxSize = NpduMaxSize - NpduMaxHeaderSize,
                            //plus microprotocol, function code and length fields (other BVLC services won't use that much)
                            BvllMaxSize = NpduMaxSize + BvllMaxHeaderSize
                                      };

    enum BacnetSegmentation {
        SegmentedBoth       = 0,
        SegmentedTransmit   = 1,
        SegmentedReceive    = 2,
        SegmentedNOT        = 3
    };

    struct  ObjectIdStruct {
        BacnetObjectType::ObjectType objectType;
        quint32 instanceNum;
    };

    quint32 objIdToNum(ObjectIdStruct &objId);
    ObjectIdStruct numToObjId(unsigned int numObjId);
    uint qHash(ObjectIdStruct objId);
    bool operator==(ObjectIdStruct one, ObjectIdStruct other);

    enum {
        ArrayIndexNotPresent = 0xffffffff,
        MaximumInstanceNumber = 0x3fffff,
        ObjectInstanceMask  = 0x3fffff,
        InvalidInstanceNumber = 0xffffffff,
        SNGVendorIdentifier = 0x00ff00ff,
        PriorityValueNotPresent = 0xff
                           };

    struct WriteAccessSpecificationStruct {

    };

    class BacnetDataInterface;
    struct PropertyValueStruct {
        BacnetProperty::Identifier propertyId;
        quint32 arrayIndex;
        BacnetDataInterface *value;
        quint8 priority;
    };

    enum NetworkPriority {
        PriorityNormal      = 0x00,
        PriorityUrgent      = 0x01,
        PriorityCritical    = 0x10,
        PriorityLifeSafety  = 0x11
                          };


};

namespace BacnetServices
{
    enum BacnetConfirmedServiceRequest {
        // Alarm and Event Services
        AcknowledgeAlarm                = 0,
        ConfirmedCOVNotification 	= 1,    //this will be supported
        ConfirmedEventNotification 	= 2,
        GetAlarmSummary                 = 3,
        GetEnrollmentSummary            = 4,
        GetEventInformation             = 29,
        SubscribeCOV                    = 5,    //this will be supported
        SubscribeCOVProperty            = 28,   //this maybe will be supported
        LifeSafetyOperation             = 27,
        // File Access Services
        AtomicReadFile                  = 6,
        AtomicWriteFile                 = 7,
        // Object Access Services
        AddListElement                  = 8,
        RemoveListElement               = 9,
        CreateObject                    = 10,
        DeleteObject                    = 11,
        ReadProperty                    = 12,   //this will be supported
        ReadPropertyConditional 	= 13,
        ReadPropertyMultiple            = 14,   //this maybe will be suppoted
        ReadRange                       = 26,
        WriteProperty                   = 15,   //this will be supported
        WritePropertyMultiple           = 16,   //this maybe will be supported
        // Remote Device Management Services
        DeviceCommunicationControl 	= 17,
        ConfirmedPrivateTransfer 	= 18,
        ConfirmedTextMessage            = 19,
        ReinitializeDevice              = 20,
        // Virtual Terminal Services
        VtOpen                          = 21,
        VtClose                         = 22,
        VtData                          = 23,
        // Security Services
        Authenticate                    = 24,
        RequestKey                      = 25
        // Services added after 1995
        // readRange 	= 26 see Object Access Services
        // lifeSafetyOperation 	= 27 see Alarm and Event Services
        // subscribeCOVProperty 	= 28 see Alarm and Event Services
        // getEventInformation 	= 29 see Alarm and Event Services
    };

    typedef BacnetConfirmedServiceRequest BacnetConfirmedServiceChoice;
    typedef BacnetConfirmedServiceRequest BacnetErrorChoice;

    enum BacnetUnconfirmedServiceRequest {
        I_Am            = 0,
        I_Have          = 1,
        UnconfirmedCOVNotification      = 2,
        UnconfirmedEventNotification    = 3,
        UnconfirmedPrivateTransfer      = 4,
        UnconfirmedTextMessage          = 5,
        TimeSynchronization             = 6,
        WhoHas                          = 7,
        WhoIs                           = 8,
        UtcTimeSynchronization          = 9
    };

    typedef BacnetUnconfirmedServiceRequest BacnetUnconfirmedServiceChoice;

}

namespace Bacnet {
    namespace AppTags {
        enum BacnetTags {
            InvalidTag      = -1,

            Null            = 0x00,
            Boolean         = 0x01,
            UnsignedInteger = 0x02,
            SignedInteger   = 0x03,
            Real            = 0x04,
            Double          = 0x05,
            OctetString     = 0x06,
            CharacterString = 0x07,
            BitString       = 0x08,
            Enumerated      = 0x09,
            Date            = 0x0a,
            Time            = 0x0b,
            BacnetObjectIdentifier = 0x0c,
            ASHRAE0         = 0x0d,
            ASHRAE1         = 0x0e,
            ASHRAE2         = 0x0f,
            LastAshraeTag   = ASHRAE2,

            ExtendedTagNumber = 0xff
                            };
    }
}

namespace Bacnet {
    //this enum should be used only internally. It is not covered by a standard.
    namespace DataType {
        enum DataType {
            //make sure the application tags correspond to their data types!
            Null            = AppTags::Null,
            BOOLEAN         = AppTags::Boolean,
            Unsigned        = AppTags::UnsignedInteger,
            Signed          = AppTags::SignedInteger,
            Real            = AppTags::Real,
            Double          = AppTags::Double,
            OctetString     = AppTags::OctetString,
            CharacterString = AppTags::CharacterString,
            BitString       = AppTags::BitString,
            Enumerated      = AppTags::Enumerated,
            Date            = AppTags::Date,
            Time            = AppTags::Time,
            BACnetObjectIdentifier = AppTags::BacnetObjectIdentifier,


            BACnetObjectType,
            BACnetDeviceStatus,
            Unsigned16,
            BACnetServicesSupported,
            BACnetObjectTypesSupported,
            BACnetSegmentation,
            BACnetVTClass,
            BACnetVTSession,
            BACnetSessionKey,
            BACnetRecipient,
            BACnetRecipientProcess,
            BACnetAddressBinding,
            BACnetAddress,
            BACnetTimeStamp,
            BACnetDateTime,
            BACnetCOVSubscription,
            BACnetPropertyRefernce,
            BACnetObjectPropertyReference,
            BACnetArray           = 0x010000,
            BACnetList            = 0x020000,
            BACnetSequence        = BACnetArray | BACnetList,
            BacnetAbstract        = 0x040000,
            InvalidType     = 0xff0000,

            MaxType = 0xffffff
                            };
    }
}

#endif // BACNETCOMMON_H
