//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/NetworkAddressImpl.hpp>
#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    NetworkAddressImpl::NetworkAddressImpl(size_t arg_maxSize)
        throw(AssertException, exception) :
        nativeSize (0), nativeMaxSize(arg_maxSize) { }

    NetworkAddressImpl::~NetworkAddressImpl() throw() { }

    const string& NetworkAddressImpl::getName() const
        throw(AssertException, exception)
    {
        return name;
    }
    
    const string& NetworkAddressImpl::getAddress(sa_family_t family) const
        throw(AssertException, exception)
    {
        Assert(family == getNativeForm()->sa_family,
               AssertException("nbpp::NetworkAddressImpl::getHostAddress(): "
                               "no default address conversion"));
        return address;
    }

    size_t NetworkAddressImpl::getNativeFormSize() const
        throw(AssertException, exception)
    {
        return nativeSize;
    }

    size_t NetworkAddressImpl::getNativeFormMaxSize() const
        throw(AssertException, exception)
    {
        return nativeMaxSize;
    }
}
