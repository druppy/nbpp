//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/UnixAddress.hpp>

namespace nbpp
{
    UnixAddress::UnixAddress() throw(exception) { }

    UnixAddress::UnixAddress(UnixAddressHandle arg_impl)
        throw(AssertException, exception) :
        NetworkAddressTemplate<UnixAddressHandle>(arg_impl) { }

    UnixAddress UnixAddress::getByPath(const string& path)
        throw(AssertException, exception)
    {
        return UnixAddress(UnixAddressImpl::getByPath(path));
    }
    
    UnixAddress UnixAddress::getFromNativeForm(const sockaddr_un& nativeForm)
        throw(AssertException, exception)
    {
        return UnixAddress(UnixAddressImpl::getFromNativeForm(nativeForm));
    }

    UnixAddress UnixAddress::allocEmptyAddress() throw(AssertException, exception)
    {
        sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        return UnixAddress(UnixAddressImpl::getFromNativeForm(addr));
    }

}
