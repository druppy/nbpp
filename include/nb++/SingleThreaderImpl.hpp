//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SINGLE_THREADER_IMPL
#define _NBPP_SINGLE_THREADER_IMPL

#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
    class SingleThreaderImpl : public ThreaderImpl
    {
    public:
        virtual void queue(Command command)
            throw(ThreadControlException, AssertException, exception);

    protected:
        virtual void doInit() throw(ThreadControlException, AssertException, exception);
    };

    typedef RefHandle<SingleThreaderImpl> SingleThreaderHandle;
}

#endif /* _NBPP_SINGLE_THREADER_IMPL */
