//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_NETWORK_ADDRESS_TEMPLATE
#define _NBPP_NETWORK_ADDRESS_TEMPLATE

#include <nb++/NetworkAddressImpl.hpp>

namespace nbpp
{
    /**
     * A base class template for classes representing network addresses.  This class is
     * not meant to be used directly; use one of its derived classes instead.
     *
     * This class is reference-counted.
     */
    template<class ImplHandle> class NetworkAddressTemplate
    {
    public:
        /**
         * Constructs a null object.
         */
        NetworkAddressTemplate() throw(exception) { }

        /**
         * Destructor.
         */
        virtual ~NetworkAddressTemplate() throw() { }

        /**
         * Gets the canonical name of the host.  If the name is unknown, returns an empty
         * string.
         */
        const string& getName() const throw(AssertException, exception)
        {
            return m_impl->getName();
        }

        /**
         * Gets the host address in native format.
         */
        const string& getAddress() const throw(AssertException, exception)
        {
            return getAddress(getNativeFormFamily());
        }

        /**
         * Converts the host address to the specified domain's
         * string representation.
         *
         * @exception AssertException if the address family requested does not correspond
         * to the type of this address.
         */
        const string& getAddress(sa_family_t family) const
            throw(AssertException, exception)
        {
            return m_impl->getAddress(family);
        }

        const sockaddr* getNativeForm() const throw(AssertException, exception)
        {
            return m_impl->getNativeForm();
        }

        sa_family_t getNativeFormFamily() const throw(AssertException, exception)
        {
            return getNativeForm()->sa_family;
        }

        size_t getNativeFormSize() const throw(AssertException, exception)
        {
            return m_impl->getNativeFormSize();
        }

        size_t getNativeFormMaxSize() const throw(AssertException, exception)
        {
            return m_impl->getNativeFormMaxSize();
        }

        void assignFromNativeForm(sockaddr* arg_address, size_t arg_len)
            throw(AssertException, exception)
        {
            return m_impl->assignFromNativeForm(arg_address, arg_len);
        }

        /*
         * Constructs an empty NetworkAddressImpl, whose contents are
         * to be filled in by some other method.
         */
        NetworkAddressImpl* makeEmptyClone() const throw(AssertException, exception)
        {
            return m_impl->makeEmptyClone();
        }

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw()
        {
            return m_impl;
        }

        // Not intended for general use
        const ImplHandle& getImpl() const throw(AssertException, exception)
        {
            return m_impl;
        }

    protected:
        mutable ImplHandle m_impl;
        NetworkAddressTemplate(const ImplHandle& arg_impl) throw(AssertException, exception) :
            m_impl(arg_impl) { }
    };
}

#endif /* _NBPP_NETWORK_ADDRESS_TEMPLATE */
