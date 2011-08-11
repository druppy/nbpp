//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/UnixAddressImpl.hpp>
#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <cstddef>

namespace nbpp
{
    UnixAddressImpl::UnixAddressImpl() throw(AssertException, exception) :
        NetworkAddressImpl(sizeof(nativeAddress))
    {
        sockaddr_un addr;
        memset (&addr, 0, sizeof (addr));
        addr.sun_family = AF_UNIX;
        assignFromNativeForm(reinterpret_cast<const sockaddr*>(&addr), sizeof (addr));
    }

    UnixAddressImpl::~UnixAddressImpl() throw() { }

    UnixAddressHandle UnixAddressImpl::getByPath(const string& path)
        throw(AssertException, exception)
    {
        sockaddr_un addr;

        Assert(path.length() >= sizeof(addr.sun_path),
               AssertException("nbpp::UnixAddressImpl::getByPath(): "
                               "path too long"));
        
        memset(&addr, 0, sizeof (addr));
        addr.sun_family = AF_UNIX;
        addr.sun_path[path.copy(addr.sun_path, sizeof(addr.sun_path) - 1)] = '\0';

        UnixAddressHandle p(new UnixAddressImpl(addr));
        p->name = p->address = path; 
        return p;
    }

    UnixAddressHandle UnixAddressImpl::getFromNativeForm(const sockaddr_un& nativeForm)
        throw(AssertException, exception)
    {
        return UnixAddressHandle(new UnixAddressImpl(nativeForm));
    }

    const sockaddr* UnixAddressImpl::getNativeForm() const
        throw(AssertException, exception)
    {
        return reinterpret_cast<const sockaddr*>(&nativeAddress);
    }

    void UnixAddressImpl::assignFromNativeForm(const sockaddr* arg_address,
                                               size_t arg_size)
        throw(AssertException, exception)
    {
        DEBUG(Assert(arg_size >= sizeof(sockaddr_un),
                     AssertException("nbpp::UnixAddressImpl::assignFromNativeForm(): "
                                     "sockaddr is too short")));
        Assert(arg_address->sa_family == AF_UNIX,
               AssertException("nbpp::UnixAddressImpl::assignFromNativeForm(): "
                               "sockaddr is not an AF_UNIX address"));
        memcpy (&nativeAddress, arg_address, sizeof(nativeAddress));
        name = address = nativeAddress.sun_path;
        nativeSize = offsetof(sockaddr_un, sun_path) + strlen(nativeAddress.sun_path);
    }


    NetworkAddressImpl* UnixAddressImpl::makeEmptyClone() const
        throw(AssertException, exception)
    {
        return new UnixAddressImpl;
    }
    
    UnixAddressImpl::UnixAddressImpl(const sockaddr_un& nativeForm)
        throw(AssertException, exception) : NetworkAddressImpl(sizeof(nativeAddress))
    {
        assignFromNativeForm(reinterpret_cast<const sockaddr*>(&nativeForm),
                             sizeof(nativeForm));
    }
}
