//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD_IMPL
#define _NBPP_THREAD_IMPL

#include <nb++/Exception.hpp> 
#include <nb++/Runnable.hpp>
#include <nb++/RefCount.hpp>

extern "C"
{
#include <pthread.h>
#include <errno.h>
}

namespace nbpp
{
    class ThreadImpl : public RefCounted
    {
    public:
        ThreadImpl(Runnable& runnable, bool arg_makeJoinable = true)
            throw(AssertException, exception);

        ~ThreadImpl() throw();
        
        void start() throw(ThreadControlException, AssertException, exception);

        void detach() throw(ThreadControlException, AssertException, exception);

        bool isJoinable() throw(ThreadControlException, AssertException, exception);
        
        void join() throw(ThreadControlException, AssertException, exception);

    private:
        ThreadImpl(const ThreadImpl&);
        ThreadImpl& operator=(const ThreadImpl&);

        Runnable& runnable;
        pthread_t thread;
        pthread_attr_t thread_attr;
        bool started;
#ifndef NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE
        bool makeJoinable;
#endif // NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE
#ifndef NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
        bool detached;
#endif // NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE

        static void* startThread(void* arg_threadimpl) throw();

		RefHandle<ThreadImpl> m_self;
    };
}

#endif /* _NBPP_THREAD_IMPL */
