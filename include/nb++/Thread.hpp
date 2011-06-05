//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD
#define _NBPP_THREAD

#include <nb++/Exception.hpp>
#include <nb++/ThreadImpl.hpp>

#include <unistd.h>

namespace nbpp
{
    /**
     * Represents a thread.  When the start() method is called, the
     * Runnable's run() method is called in the new thread.  The thread
     * terminates when the Runnable's run() method returns.  This class is
     * reference-counted.
     */
    class Thread
    {
    public:
        /**
         * Constructs a Thread object.
         * @param runnable the Runnable whose run() method will be called
         * when the thread is started.
         * @param makeJoinable true if the new thread should be joinable,
         * false if it should be detached.
         */
        Thread(Runnable& runnable, bool makeJoinable = true)
            throw(AssertException, exception);

        ~Thread() throw();
        
        /**
         * Spawns a new thread, and calls the Runnable's run() method in
         * it.  Can only be called once on each Thread object; subsequent
         * calls have no effect.  If the thread is joinable, its resources
         * will be kept when the Runnable's run() method returns, until
         * another thread calls join().
         *
         * @exception ThreadControlException if the operation cannot be performed.
         */
        void start() throw(ThreadControlException, AssertException, exception);

        /**
         * Detaches the thread, if it's joinable, otherwise does nothing.
         *
         * @exception ThreadControlException
         */
        void detach() throw(ThreadControlException, AssertException, exception);

        /**
         * @return true if the thread is joinable.
         *
         * @exception ThreadControlException if the operation cannot be performed.
         */
        bool isJoinable() throw(ThreadControlException, AssertException, exception);
        
        /**
         * If the thread is joinable, this method blocks until the
         * Runnable's run() method returns, then releases the thread's
         * resources.
         *
         * @exception ThreadControlException if the operation cannot be performed.
         */
        void join() throw(ThreadControlException, AssertException, exception);

        /**
         * Constructs a null Thread object.
         */
        Thread() throw(exception);

		/**
		   I thin static wrapper on the posix sleep function, to make it possible
		   to put the calling (and current) thread asleep.

		   @param nSeconds is the number of seconds to sleep
		 */
		static void sleep( unsigned int nSeconds ) { ::sleep( nSeconds );}

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw();

    private:
        RefHandle<ThreadImpl> impl;
    };
}

#endif /* _NBPP_THREAD */
