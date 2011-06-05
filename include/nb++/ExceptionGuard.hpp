//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_EXCEPTION_GUARD
#define _NBPP_EXCEPTION_GUARD

#include <nb++/Exception.hpp>

namespace nbpp
{
    using std::unexpected_handler;

    extern void assertUnexpected() throw(UnexpectedException);

    /**
     * Catches unexpected exceptions in a program, and rethrows them as
     * UnexpectedException objects.  UnexpectedException is derived from AssertException,
     * which nb++ passes to user code.  If the unexpected exception is derived from
     * Exception or std::exception, the UnexpectedException object's detail
     * message will contain a description of it.
     *
     * To use this feature, just instantiate an ExceptionGuard as a local variable at the
     * beginning of your program's main() method.
     *
     * This class is not reference-counted.
     *
     * @see UnexpectedException
     */
    class ExceptionGuard
    {
        unexpected_handler old;

    public:
        /**
         * Sets an unexpected_handler that rethrows exceptions as UnexpectedException
         * objects.
         */
        ExceptionGuard() throw();

        /**
         * Restores the original unexpected_handler.
         */
        ~ExceptionGuard() throw();
    };
}

#endif /* _NBPP_EXCEPTION_GUARD */
