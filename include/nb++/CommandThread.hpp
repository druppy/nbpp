//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_COMMAND_THREAD
#define _NBPP_COMMAND_THREAD

#include <nb++/Exception.hpp>
#include <nb++/Thread.hpp>
#include <nb++/ThreadObserver.hpp>
#include <nb++/CommandQueue.hpp>

namespace nbpp
{
    class CommandThreadImpl : public Runnable, public RefCounted
    {
    public:
        CommandThreadImpl(ThreadObserver& observer, CommandQueue commandQueue,
                          unsigned int maxRequests) throw(AssertException, exception);

        virtual ~CommandThreadImpl() throw();

        void start() throw(ThreadControlException, AssertException, exception);

        void cancel() throw(AssertException, exception);

        virtual void run() throw();

        bool isDone() const throw(AssertException, exception);

        bool wasCanceled() const throw(AssertException, exception);

    private:
        Thread thread;
        ThreadObserver& observer;
        CommandQueue commandQueue;
        unsigned int maxRequests;
        bool done;
        bool canceled;
    };


    /**
     * A thread that reads and executes Command objects from a CommandQueue,
     * and reports to a ThreadObserver.
     *
     * This class is reference-counted.
     *
     * @see Command
     * @see CommandQueue
     * @see ThreadObserver
     */
    class CommandThread
    {
    public:
        /**
         * Constructs a CommandThread.
         *
         * @param observer a ThreadObserver to which this thread should report changes
         * in its state.
         * @param commandQueue a CommandQueue from which this thread should get
         * commands to execute.
         * @param maxRequests the maximum number of commands that this thread
         * should execute before terminating.
         */
        CommandThread(ThreadObserver& observer, CommandQueue commandQueue,
                      unsigned int maxRequests) throw(AssertException, exception);

        /**
         * Starts the thread.
         */
        void start() throw(ThreadControlException, AssertException, exception);

        /**
         * Waits until the thread has completed its current request, then
         * stops it.
         */
        void cancel() throw(AssertException, exception);

        /**
         * @return true if this thread has completed its life cycle.
         */
        bool isDone() const throw(AssertException, exception);

        /**
         * @return true if this thread was canceled.
         */
        bool wasCanceled() const throw(AssertException, exception);

    private:
        RefHandle<CommandThreadImpl> impl;
    };
}

#endif /* _NBPP_COMMAND_THREAD */
