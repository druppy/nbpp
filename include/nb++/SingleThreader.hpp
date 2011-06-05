//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SINGLE_THREADER
#define _NBPP_SINGLE_THREADER

#include <nb++/Threader.hpp>
#include <nb++/SingleThreaderImpl.hpp>

namespace nbpp
{
    /**
     * A Threader that executes Command objects in a single thread.
     *
     * This class is reference-counted.
     *
     * @see Threader
     */
    class SingleThreader : public ThreaderTemplate<SingleThreaderHandle>
    {
    public:
        /**
         * Constructs a SingleThreader.
         */
        SingleThreader() throw(AssertException, exception);
    };
}

#endif /* _NBPP_SINGLE_THREADER */
