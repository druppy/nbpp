//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_FORK_THREADER
#define _NBPP_FORK_THREADER

#include <nb++/Threader.hpp>
#include <nb++/ForkThreaderImpl.hpp>

namespace nbpp
{
    /**
     * A Threader that executes each Command in a new child process.
     *
     * Note: ForkThreader's implementation of queue() calls ::exit(0) in the child process
     * after Command::execute() returns.  Destructors for any remaining non-static objects
     * will therefore not be called.
     *
     * This class is reference-counted.
     *
     * @see Threader
     */
    class ForkThreader : public ThreaderTemplate<ForkThreaderHandle>
    {
    public:
        /**
         * Constructs a ForkThreader.
         */
        ForkThreader() throw(AssertException, exception);
    };
}

#endif /* _NBPP_FORK_THREADER */
