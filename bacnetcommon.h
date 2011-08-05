#ifndef BACNETCOMMON_H
#define BACNETCOMMON_H

#include <stdio.h>
#include <QtCore>

/**
  This header is meant to provide some common data for all BACnet classes within the project.
  */
namespace BacnetCommon
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
