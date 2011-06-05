//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_INET_ADDRESS_IMPL
#define _NBPP_INET_ADDRESS_IMPL

#include <nb++/Exception.hpp>
#include <nb++/Mutex.hpp>
#include <nb++/NetworkAddressImpl.hpp>

extern "C"
{
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
}

namespace nbpp
{
    using std::string;

    class InetAddressImpl;
    typedef RefHandle<InetAddressImpl> InetAddressHandle;

    class InetAddressImpl : public NetworkAddressImpl
    {
    public:
        virtual ~InetAddressImpl() throw();

        static InetAddressHandle getByName(const string& hostName,
                                           unsigned short port)
            throw(UnknownHostException, AssertException, exception);
        
        static InetAddressHandle getFromNativeForm(const sockaddr_in& nativeForm)
            throw(AssertException, exception);

        static InetAddressHandle getFromNativeForm(const in_addr& nativeForm,
                                                   unsigned short nativePort,
                                                   const string& hostName = "")
            throw(AssertException, exception);

        static InetAddressHandle getAnyLocalHost(unsigned short port)
            throw(AssertException, exception);

        virtual const sockaddr* getNativeForm() const throw(AssertException, exception);

        virtual void assignFromNativeForm(const sockaddr* address, size_t size)
            throw(AssertException, exception);

        virtual NetworkAddressImpl* makeEmptyClone() const
            throw(AssertException, exception);

        unsigned short getPort() const throw(AssertException, exception);

    private:
        InetAddressImpl() throw(AssertException, exception);
        InetAddressImpl(const sockaddr_in& nativeForm) throw(AssertException, exception);

        sockaddr_in nativeAddress;
    };
}

#endif /* _NBPP_INET_ADDRESS_IMPL */
