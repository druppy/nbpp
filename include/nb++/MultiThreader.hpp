//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_MULTI_THREADER
#define _NBPP_MULTI_THREADER

#include <nb++/Threader.hpp>
#include <nb++/MultiThreaderImpl.hpp>

namespace nbpp
{
    /**
     * A Threader that dispatches Command objects to a ThreadPool.
     *
     * This class is reference-counted.
     *
     * @see Threader
     */
    class MultiThreader : public ThreaderTemplate<MultiThreaderHandle>
    {
    public:
        /**
         * Constructs a MultiThreader.
         */
        MultiThreader() throw(AssertException, exception);

        /**
         * @return the ThreadPool maintained by this MultiThreader.
         */
        ThreadPool& getThreadPool() throw(AssertException, exception);
    };
}

#endif /* _NBPP_MULTI_THREADER */
