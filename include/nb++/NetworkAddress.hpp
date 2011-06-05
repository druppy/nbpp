//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_NETWORK_ADDRESS
#define _NBPP_NETWORK_ADDRESS

#include <nb++/NetworkAddressImpl.hpp>
#include <nb++/NetworkAddressTemplate.hpp>

namespace nbpp
{
    /**
     * Represents a generic network address.  NetworkAddress doesn't
     * have real derived classes, but implicit conversion of
     * specialized address types to NetworkAddress is supported.
     *
     * This class is reference-counted.
     */
    class NetworkAddress : public NetworkAddressTemplate<NetworkAddressHandle>
    {
    public:
        /**
         * Constructs a null object.
         */
        NetworkAddress() throw(exception);

        // Not intended for general use
        template<class C> NetworkAddress(const C& addr)
            throw(AssertException, exception) :
            NetworkAddressTemplate<NetworkAddressHandle>(addr.getImpl()) { }

        // Not intended for general use
        NetworkAddress(NetworkAddressImpl* impl) throw(AssertException, exception);
    };

    /**
     * Returns true if two NetworkAddress objects represent the same
     * address.
     *
     * @return true if the two NetworkAddress objects represent the same
     * address.
     */
    bool operator==(NetworkAddress& left, NetworkAddress& right)
        throw(AssertException, exception);
}

#endif /* _NBPP_NETWORK_ADDRESS */
