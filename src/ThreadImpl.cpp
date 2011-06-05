//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ThreadImpl.hpp>
#include <nb++/Thread.hpp>

namespace nbpp
{
    ThreadImpl::ThreadImpl(Runnable& arg_runnable, bool arg_makeJoinable)
        throw(AssertException, exception) :
        runnable(arg_runnable), started(false)
#ifndef NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
        , detached (false)
#endif // NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
    {
        pthread_attr_init(&thread_attr);
#ifdef NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE
        if (!arg_makeJoinable)
            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
#else // !NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE
        makeJoinable = arg_makeJoinable;
#endif // !NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE
    }

    ThreadImpl::~ThreadImpl() throw()
    {
        pthread_attr_destroy(&thread_attr);
    }

    void ThreadImpl::start() throw(ThreadControlException, AssertException, exception)
    {
        if (!started) {
			int err, retry = 3;
            started = true;

            while((err = pthread_create( &thread, &thread_attr, &startThread, this ))) {
				if( err == EAGAIN ) {
					Thread::sleep( 1 );
					if( retry ) {
						retry--;
						continue;
					}
				}
				throw ThreadControlException(strerror_r( err ));
			}

			m_self = this; // Take my own refcount, to be released inside the startThread function.
        }
    }

    void ThreadImpl::detach() throw(ThreadControlException, AssertException, exception)
    {
        if (pthread_detach(thread)) {
            throw ThreadControlException(strerror_r(errno));
        }
#ifndef NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
        detached = true;
#endif // !NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
    }

    bool ThreadImpl::isJoinable() throw(ThreadControlException, AssertException, exception)
    {
#ifdef NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
        int detachState;

        if (pthread_attr_getdetachstate(&thread_attr, &detachState)) {
            throw ThreadControlException(strerror_r(errno));
        }

        return (detachState == PTHREAD_CREATE_JOINABLE);
#else // !NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
        return !detached;
#endif // !NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE
    }

    void ThreadImpl::join() throw(ThreadControlException, AssertException, exception)
    {
        if (pthread_join(thread, NULL)) {
            throw ThreadControlException(strerror_r(errno));
        }
    }

    void* ThreadImpl::startThread(void* arg_threadimpl) throw()
    {
        try {
            ThreadImpl* thread = static_cast<ThreadImpl *>(arg_threadimpl);

			RefHandle<ThreadImpl> ref( thread );  // Make sure not to delete running thread class

			thread->m_self = NULL;

#ifndef NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE

            if (!thread->makeJoinable)
                thread->detach ();

#endif // !NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE

            thread->runnable.run();
        } catch ( ... ) {
			std::cerr << "Unexpected ... " << std::endl;
		}

        return 0;
    }
}
