#ifndef BACNET_DISCOVERYWRAPPER_H
#define BACNET_DISCOVERYWRAPPER_H

namespace Bacnet {

class DiscoveryWrapper
{
public:
    DiscoveryWrapper();
};

class ConfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{

};

class UnconfirmedDiscoveryWrapper:
        public DiscoveryWrapper
{

};


} // namespace Bacnet

#endif // BACNET_DISCOVERYWRAPPER_H
