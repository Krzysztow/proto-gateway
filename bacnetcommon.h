#ifndef BACNETCOMMON_H
#define BACNETCOMMON_H

#include <stdio.h>
#include <QtCore>

/**
  This header is meant to provide some common data for all BACnet classes within the project.
  */
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

    enum BacnetObjectType {
        ObjectTypeAccumulator 	= 23,
        ObjectTypeAnalogInput 	= 0,
        ObjectTypeAnalogOutput 	= 1,
        ObjectTypeAnalogValue 	= 2,
        ObjectTypeAveraging 	= 18,
        ObjectTypeBinaryInput 	= 3,
        ObjectTypeBinaryOutput 	= 4,
        ObjectTypeBinaryValue 	= 5,
        ObjectTypeCalendar 	= 6,
        ObjectTypeCommand 	= 7,
        ObjectTypeDevice 	= 8,
        ObjectTypeEventEnrollment 	= 9,
        ObjectTypeFile 	= 10,
        ObjectTypeGroup 	= 11,
        ObjectTypeLifeSafetyPoint 	= 21,
        ObjectTypeLifeSafetyZone 	= 22,
        ObjectTypeLoop 	= 12,
        ObjectTypeMultiStateInput 	= 13,
        ObjectTypeMultiStateOutput 	= 14,
        ObjectTypeMultiStateValue 	= 19,
        ObjectTypeNotificationClass 	= 15,
        ObjectTypeProgram 	= 16,
        ObjectTypePulseConverter 	= 24,
        ObjectTypeSchedule 	= 17,
        // see averaging 	= 18,
        // see multiStateValue 	= 19,
        ObjectTypeTrendLog 	= 20,
        // see lifeSafetyPoint 	= 21,
        // see lifeSafetyZone 	= 22,
        // see accumulator 	= 23,
        // see pulseConverter 	= 24
        ObjectTypeUndefined = 255//not defiend by standard, but we have space for it - only 10 bits are used
    };

    struct ObjectId {
        BacnetObjectType objectType;
        quint32 instanceNum;
    };

    enum NetworkPriority {
        PriorityNormal      = 0x00,
        PriorityUrgent      = 0x01,
        PriorityCritical    = 0x10,
        PriorityLifeSafety  = 0x11
                          };
};

namespace BacnetConfirmedService
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
        RequestKey                      = 25,
        // Services added after 1995
        // readRange 	= 26 see Object Access Services
        // lifeSafetyOperation 	= 27 see Alarm and Event Services
        // subscribeCOVProperty 	= 28 see Alarm and Event Services
        // getEventInformation 	= 29 see Alarm and Event Services
    };

    typedef BacnetConfirmedServiceRequest BacnetConfirmedServiceChoice;
}

#endif // BACNETCOMMON_H
