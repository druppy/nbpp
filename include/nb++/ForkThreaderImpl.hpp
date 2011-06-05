//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_FORK_THREADER_IMPL
#define _NBPP_FORK_THREADER_IMPL

#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
    class ForkThreaderImpl : public ThreaderImpl
    {
    public:
        virtual ~ForkThreaderImpl() throw();

        virtual void queue(Command command)
            throw(ThreadControlException, AssertException, exception);

    protected:
        virtual void doInit() throw(ThreadControlException, AssertException, exception);
    };

    typedef RefHandle<ForkThreaderImpl> ForkThreaderHandle;
}

#endif /* _NBPP_FORK_THREADER_IMPL */
