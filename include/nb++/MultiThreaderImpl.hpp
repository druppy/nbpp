//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_MULTI_THREADER_IMPL
#define _NBPP_MULTI_THREADER_IMPL

#include <nb++/ThreaderImpl.hpp>
#include <nb++/ThreadPool.hpp>

namespace nbpp
{
    class MultiThreaderImpl : public ThreaderImpl
    {
    public:
        virtual ~MultiThreaderImpl() throw();

        virtual void queue(Command command)
            throw(ThreadControlException, AssertException, exception);

        ThreadPool& getThreadPool() throw(AssertException, exception);

    protected:
        virtual void doInit() throw(ThreadControlException, AssertException, exception);

    private:
        ThreadPool pool;
    };

    typedef RefHandle<MultiThreaderImpl> MultiThreaderHandle;
}

#endif /* _NBPP_MULTI_THREADER_IMPL */
