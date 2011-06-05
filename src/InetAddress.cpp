//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/InetAddress.hpp>

namespace nbpp
{
    InetAddress::InetAddress() throw(exception) { }

    InetAddress::InetAddress(InetAddressHandle arg_impl)
        throw(AssertException, exception) :
        NetworkAddressTemplate<InetAddressHandle>(arg_impl) { }

    InetAddress InetAddress::getByName(const string& hostName, unsigned short port)
        throw(UnknownHostException, AssertException, exception)
    {
        return InetAddress(InetAddressImpl::getByName(hostName, port));
    }
    
    InetAddress InetAddress::getFromNativeForm(const sockaddr_in& nativeForm)
        throw(AssertException, exception)
    {
        return InetAddress(InetAddressImpl::getFromNativeForm(nativeForm));
    }

    InetAddress InetAddress::getFromNativeForm(const in_addr& nativeForm,
                                               unsigned short nativePort)
        throw(AssertException, exception)
    {
        return InetAddress(InetAddressImpl::getFromNativeForm(nativeForm, nativePort));
    }

    InetAddress InetAddress::getAnyLocalHost(unsigned short port)
        throw(AssertException, exception)
    {
        return InetAddress(InetAddressImpl::getAnyLocalHost(port));
    }
    
    std::list<InetAddress> InetAddress::getAllByName(const string& hostName,
																 unsigned short port)
        throw(UnknownHostException, AssertException, exception)
    {
        hostent hostbuf, *host;
        char buf [2048];
        int h_err;

        if (gethostbyname_r(hostName, &hostbuf, buf, sizeof (buf), &host, &h_err) == -1)
            host = NULL;
        
        if ( !host )
            throw UnknownHostException(h_err, hstrerror_r(h_err));

        std::list<InetAddress> addrList;

        // Make an InetAddress for each IP address associated with this
        // hostname.
        in_addr addr;
        for (int i = 0; host->h_addr_list[i]; ++i)
        {
            memcpy(&addr, host->h_addr_list[i], sizeof(addr));
            InetAddressHandle p = InetAddressImpl::getFromNativeForm(addr, port,
                                                                     host->h_name);
            addrList.push_back( InetAddress( p ) );
        }

        return addrList;
    }

    InetAddress InetAddress::allocEmptyAddress() throw(AssertException, exception)
    {
        sockaddr_in addr;
        memset (&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        return InetAddress(InetAddressImpl::getFromNativeForm(addr));
    }

    unsigned short InetAddress::getPort() const throw(AssertException, exception)
    {
        return m_impl->getPort();
    }
}
