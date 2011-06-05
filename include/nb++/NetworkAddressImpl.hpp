//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_NETWORK_ADDRESS_IMPL
#define _NBPP_NETWORK_ADDRESS_IMPL

#include <nb++/RefCount.hpp>
#include <nb++/Exception.hpp>

extern "C"
{
#include <sys/socket.h>
}

namespace nbpp
{
    using std::string;

    class NetworkAddressImpl;
    typedef RefHandle<NetworkAddressImpl> NetworkAddressHandle;

    class NetworkAddressImpl : public RefCounted
    {
    public:
        NetworkAddressImpl(size_t arg_maxSize) throw(AssertException, exception);

        virtual ~NetworkAddressImpl() throw();

        const string& getName() const throw(AssertException, exception);

        virtual const string& getAddress(sa_family_t family) const
            throw(AssertException, exception);

        virtual const sockaddr* getNativeForm() const
            throw(AssertException, exception) = 0;

        virtual void assignFromNativeForm(const sockaddr* address, size_t size)
            throw(AssertException, exception) = 0;

        size_t getNativeFormSize() const throw(AssertException, exception);

        size_t getNativeFormMaxSize() const throw(AssertException, exception);

        virtual NetworkAddressImpl* makeEmptyClone() const
            throw(AssertException, exception) = 0;

    protected:
        string name;
        string address;
        size_t nativeSize;
        size_t nativeMaxSize;
    };
}

#endif /* _NBPP_NETWORK_ADDRESS_IMPL */
