//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_UNIX_ADDRESS_IMPL
#define _NBPP_UNIX_ADDRESS_IMPL

#include <nb++/osdep.hpp>

extern "C"
{
#include <sys/un.h>
}

#include "NetworkAddressImpl.hpp"

namespace nbpp
{
    using std::string;

    class UnixAddressImpl;
    typedef RefHandle<UnixAddressImpl> UnixAddressHandle;

    class UnixAddressImpl : public NetworkAddressImpl
    {
    public:
        virtual ~UnixAddressImpl() throw();

        static UnixAddressHandle getByPath(const string& path)
            throw(AssertException, exception);

        static UnixAddressHandle getFromNativeForm(const sockaddr_un& nativeForm)
            throw(AssertException, exception);

        virtual const sockaddr* getNativeForm() const throw(AssertException, exception);

        virtual void assignFromNativeForm(const sockaddr* address, size_t size)
            throw(AssertException, exception);

        virtual NetworkAddressImpl* makeEmptyClone() const
            throw(AssertException, exception);

    private:
        UnixAddressImpl() throw(AssertException, exception);
        UnixAddressImpl(const sockaddr_un& nativeForm) throw(AssertException, exception);

        sockaddr_un nativeAddress;
    };
}

#endif /* _NBPP_UNIX_ADDRESS_IMPL */
