//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/NetworkAddress.hpp>

namespace nbpp
{
    NetworkAddress::NetworkAddress() throw(exception) { }

    NetworkAddress::NetworkAddress(NetworkAddressImpl* arg_impl)
        throw(AssertException, exception) :
        NetworkAddressTemplate<NetworkAddressHandle>(NetworkAddressHandle(arg_impl)) { }

    bool operator==(NetworkAddress left, NetworkAddress right)
        throw(AssertException, exception)
    {
        return (left.getAddress() == right.getAddress());
    }
}
