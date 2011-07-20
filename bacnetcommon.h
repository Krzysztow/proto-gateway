#ifndef BACNETCOMMON_H
#define BACNETCOMMON_H

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
        //to be on the safe side, subtract maximum NpduHeaderSize
        ApduMaxSize = NpduMaxSize - NpduMaxHeaderSize,
        //plus microprotocol, function code and length fields (other BVLC services won't use that much)
        BvllMaxSize = NpduMaxSize + 1 + 1 + 2
    };
};

#endif // BACNETCOMMON_H
