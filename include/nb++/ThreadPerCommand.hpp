//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD_PER_COMMAND
#define _NBPP_THREAD_PER_COMMAND

#include <nb++/Threader.hpp>
#include <nb++/ThreadPerCommandImpl.hpp>

namespace nbpp
{
    /**
     * A Threader that executes each Command in a new thread.
     *
     * This class is reference-counted.
     *
     * @see Threader
     */
    class ThreadPerCommand : public ThreaderTemplate<ThreadPerCommandHandle>
    {
    public:
        /**
         * Constructs a ThreadPerCommand.
         */
        ThreadPerCommand() throw(AssertException, exception);
    };
}

#endif /* _NBPP_THREAD_PER_COMMAND */
