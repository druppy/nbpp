//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_UNIX_ADDRESS
#define _NBPP_UNIX_ADDRESS

#include <nb++/NetworkAddressTemplate.hpp>
#include <nb++/UnixAddressImpl.hpp>

namespace nbpp
{
    /**
     * Represents a Unix-domain address.  Use one of the static factory
     * methods to instantiate.  This class is reference-counted.
     */
    class UnixAddress : public NetworkAddressTemplate<UnixAddressHandle>
    {
    public:
        /**
         * Constructs a new UnixAddress representing the specified path.
         *
         * @exception AssertException if the path is too long.
         */
        static UnixAddress getByPath(const string& path) throw(AssertException, exception);

        static UnixAddress getFromNativeForm(const sockaddr_un& nativeForm)
            throw(AssertException, exception);

        /**
         * Constructs an empty (but non-null) UnixAddress, whose contents are to be filled
         * in by some other method.
         */
        static UnixAddress allocEmptyAddress() throw(AssertException, exception);

        /**
         * Constructs a null UnixAddress.
         */
        UnixAddress() throw(exception);

    private:
        UnixAddress(UnixAddressHandle impl) throw(AssertException, exception);
    };
}

#endif /* _NBPP_UNIX_ADDRESS */
