//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD_POOL
#define _NBPP_THREAD_POOL

#include <nb++/CommandThread.hpp>
#include <nb++/CommandQueue.hpp>
#include <nb++/ThreadObserver.hpp>

#include <list>

namespace nbpp
{
    using std::list;

    /**
     * A pool of CommandThread objects.  Maintains a CommandQueue which it uses to forward
     * Command objects to its threads.  As commands are added to the queue, and as threads
     * report their status, a ThreadPool adjusts the number of threads to within specified
     * limits.  By default, it starts with 5 threads, and maintains between 5 and 10 idle
     * threads.
     *
     * This class is not reference-counted.
     *
     * @see Command
     * @see CommandQueue
     * @see CommandThread
     */
    class ThreadPool : public ThreadObserver
    {
    public:
        /**
         * Constructs a ThreadPool.
         */
        ThreadPool() throw(AssertException, exception);

        /**
         * Waits for any threads that are still working to complete their
         * current request, then deletes all the threads in the pool.
         */
        ~ThreadPool() throw();

        /**
         * Starts the number of threads specified by setStartThreads(), or 5 by default.
         * Must be called before any calls to push().
         */
        void init() throw(ThreadControlException, AssertException, exception);

        /**
         * Adds a Command to the ThreadPool's CommandQueue, adjusting the
         * pool's size if necessary.
         */
        void queue(const Command& cmd)
            throw(ThreadControlException, AssertException, exception);

        /**
         * @return the CommandQueue maintained by this ThreadPool.  This method
         * is meant to be used by threads in the pool.
         */
        CommandQueue getCommandQueue() const throw(AssertException, exception);

        /**
         * @return the number of currently idle threads.
         */
        unsigned int getIdleThreads() const throw(AssertException, exception);

        /**
         * The number of threads to be started by the init() method.
         */
        unsigned int getStartThreads() const throw(AssertException, exception);

        /**
         * Sets the number of threads to be started by the init() method.
         *
         * @return this ThreadPool.
         */
        ThreadPool& setStartThreads(unsigned int arg_startThreads)
            throw(AssertException, exception);

        /**
         * @return the minimum number of idle threads to be maintained in the pool.
         */
        unsigned int getMinSpare() const throw(AssertException, exception);

        /**
         * Sets the minimum number of idle threads to be maintained in the pool.
         */
        ThreadPool& setMinSpare(unsigned int arg_minSpare)
            throw(AssertException, exception);

        /**
         * @return the maximum number of idle threads to be maintained in the pool.
         */
        unsigned int getMaxSpare() const throw(AssertException, exception);

        /**
         * Sets the maximum number of idle threads to be maintained in the pool.
         */
        ThreadPool& setMaxSpare(unsigned int arg_maxSpare)
            throw(AssertException, exception);

        /**
         * @return the maximum number of requests that each thread should handle before
         * being deleted from the pool.
         */
        unsigned int getMaxRequestsPerThread() const throw(AssertException, exception);

        /**
         * Sets the maximum number of requests that each thread should handle before being
         * deleted from the pool.  Changing this value only affects new threads.
         */
        ThreadPool& setMaxRequestsPerThread(unsigned int arg_maxRequestsPerThread)
            throw(AssertException, exception);

        /**
         * @return the maximum number of threads allowed in the pool.
         */
        unsigned int getMaxThreads(unsigned int arg_maxThreads) const
            throw(AssertException, exception);

        /**
         * Sets the maximum number of threads allowed in the pool.  Defaults to 15.
         */
        ThreadPool& setMaxThreads(unsigned int arg_maxThreads)
            throw(AssertException, exception);

        /**
         * Called by a thread to indicate a change in its state.
         *
         * @see ThreadObserver
         */
        virtual void switchState(State oldState, State newState)
            throw(AssertException, exception);

        /**
         * Called by an idle thread to ask whether it should start waiting
         * for something to do.
         *
         * @return true if the thread should wait, false if it should terminate.
         * @see ThreadObserver
         */
        virtual bool canWait() throw(AssertException, exception);

    private:
        CommandQueue commandQueue;
        mutable Mutex mutex;
    
        // Parameters
        unsigned int startThreads;
        unsigned int minSpare;
        unsigned int maxSpare;
        unsigned int maxRequestsPerThread;
        unsigned int maxThreads;

        // Current values
        unsigned int idleThreads;
        bool mustCleanUp;

        typedef list<CommandThread> ThreadList;
        ThreadList threads;

        ThreadPool(const ThreadPool&);
        ThreadPool& operator=(const ThreadPool&);
    };
}

#endif /* _NBPP_THREAD_POOL */
