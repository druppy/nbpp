//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_RUNNABLE
#define _NBPP_RUNNABLE

#include <nb++/osdep.hpp>

namespace nbpp
{
    /**
     * An interface for objects that can be run in their own thread by a
     * Thread object.
     */
    class Runnable
    {
    public:
        /**
         * Destructor.
         */
        virtual ~Runnable() throw() { }

        /**
         * Called when a thread is started.
         */
        virtual void run() throw() = 0;
    };
}

#endif /* _NBPP_RUNNABLE */
