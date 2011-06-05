//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Assert.hpp>
#include <nb++/InetAddressImpl.hpp>
#include <nb++/InetAddress.hpp>

#include <iostream>

namespace nbpp
{
    InetAddressImpl::InetAddressImpl() throw(AssertException, exception) :
        NetworkAddressImpl(sizeof (nativeAddress))
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        assignFromNativeForm(reinterpret_cast<const sockaddr*>(&addr), sizeof (addr));
    }

    InetAddressImpl::~InetAddressImpl() throw() { }

    InetAddressHandle InetAddressImpl::getByName(const string& hostName,
                                                 unsigned short port)
        throw(UnknownHostException, AssertException, exception)
    {
        hostent hostbuf, *host = &hostbuf;
        char buf [2048];
        int h_err = 0;

        if (gethostbyname_r(hostName, &hostbuf, buf, sizeof (buf),
                            &host, &h_err) == -1)
            host = NULL;
        
        if (!host)
        {
            throw UnknownHostException(h_err, hstrerror_r(h_err));
        }
        
        // Get the first IP address associated with this hostname.
        in_addr addr;
        memcpy(&addr, host->h_addr_list[0], sizeof(addr));

        return getFromNativeForm(addr, port, host->h_name);
    }

    InetAddressHandle InetAddressImpl::getFromNativeForm(const sockaddr_in& nativeForm)
        throw (AssertException, exception)
    {
        return InetAddressHandle(new InetAddressImpl(nativeForm));
    }
    
    InetAddressHandle InetAddressImpl::getFromNativeForm(const in_addr& nativeForm,
                                                         unsigned short nativePort,
                                                         const string& hostName)
        throw(AssertException, exception)
    {
        sockaddr_in addr;

        memset (&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons (nativePort);
        addr.sin_addr   = nativeForm;

        InetAddressImpl* p = new InetAddressImpl(addr);
        p->name = hostName;

        return InetAddressHandle(p);
    }

    const sockaddr* InetAddressImpl::getNativeForm() const
        throw(AssertException, exception)
    {
        return reinterpret_cast<const sockaddr*>(&nativeAddress);
    }

    InetAddressHandle InetAddressImpl::getAnyLocalHost(unsigned short port)
        throw(AssertException, exception)
    {
        struct in_addr addr;
        addr.s_addr = htonl(INADDR_ANY);
        return getFromNativeForm(addr, port);
    }

    void InetAddressImpl::assignFromNativeForm(const sockaddr* arg_address,
                                               size_t arg_size)
        throw(AssertException, exception)
    {
        DEBUG(Assert(arg_size >= sizeof (sockaddr_in),
                     AssertException("nbpp::InetAddressImpl::assignFromNativeForm(): "
                                     "sockaddr too short")));
        Assert(arg_address->sa_family == AF_INET,
               AssertException("nbpp::InetAddressImpl::assignFromNativeForm(): "
                               "sockaddr is not an AF_INET address"));
        memcpy(&nativeAddress, arg_address, sizeof (nativeAddress));
        address = inet_ntoa(nativeAddress.sin_addr);
        nativeSize = sizeof(nativeAddress);
    }

    NetworkAddressImpl* InetAddressImpl::makeEmptyClone() const
        throw(AssertException, exception)
    {
        return new InetAddressImpl;
    }

    unsigned short InetAddressImpl::getPort() const throw(AssertException, exception)
    {
        return ntohs(nativeAddress.sin_port);
    }

    InetAddressImpl::InetAddressImpl(const sockaddr_in& nativeForm)
        throw(AssertException, exception) :
        NetworkAddressImpl(sizeof (nativeAddress))
    {
        assignFromNativeForm(reinterpret_cast<const sockaddr*>(&nativeForm),
                             sizeof(nativeForm));
    }
}
