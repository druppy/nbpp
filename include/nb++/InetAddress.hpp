//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_INET_ADDRESS
#define _NBPP_INET_ADDRESS

#include <nb++/NetworkAddressTemplate.hpp>
#include <nb++/InetAddressImpl.hpp>
#include <nb++/RefCount.hpp>

#include <list> // STL

namespace nbpp
{
    /**
     * Represents an Internet address.  Use one of the static factory
     * methods to instantiate.  This class is reference-counted.
     */
    class InetAddress : public NetworkAddressTemplate<InetAddressHandle>
    {
    public:
        /**
         * Constructs a new InetAddress representing the specified
         * hostname (or IP in dotted-decimal format).
         *
         * @exception UnknownHostException if the host name cannot be
         * resolved.
         */
        static InetAddress getByName(const string& host, unsigned short port = 0)
            throw(UnknownHostException, AssertException, exception);

        /**
         * @return a List of addresses registered for the specified
         * hostname (or IP in dotted-decimal format).
         *
         * @exception UnknownHostException if the host name cannot be
         * resolved.
         */
        static std::list<InetAddress> getAllByName(const string& hostName,
												   unsigned short port = 0)
            throw(UnknownHostException, AssertException, exception);

        static InetAddress getFromNativeForm(const sockaddr_in& nativeForm)
            throw(AssertException, exception);

        static InetAddress getFromNativeForm(const in_addr& nativeForm,
                                             unsigned short nativePort = 0)
            throw(AssertException, exception);

        /**
         * Constructs a wildcard (INADDR_ANY) InetAddress representing the local host.
         */
        static InetAddress getAnyLocalHost(unsigned short port = 0)
            throw(AssertException, exception);

        /**
         * Constructs an empty (but non-null) InetAddress, whose contents are to be filled
         * in by some other method.
         */
        static InetAddress allocEmptyAddress() throw(AssertException, exception);

        /**
         * @return the port component of this InetAddress.
         */
        unsigned short getPort() const throw(AssertException, exception);

        /**
         * Constructs a null InetAddress.
         */
        InetAddress() throw(exception);

    private:
        InetAddress(InetAddressHandle impl) throw(AssertException, exception);
    };
}

#endif /* _NBPP_INET_ADDRESS */
