//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_THREAD_OBSERVER
#define _NBPP_THREAD_OBSERVER

#include <nb++/osdep.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    /**
     * An interface for objects that keep track of the current state of one or
     * more threads.
     *
     * @see ThreadState
     * @see CommandThread
     */
    class ThreadObserver
    {       
    public:
        /**
         * Constants representing the state of a thread.
         */
        enum State
        {
            Idle,      /**< The thread isn't doing anything. */
            Waiting,   /**< The thread is waiting for something to do. */
            Working,   /**< The thread is busy.  */
            Finished   /**< The thread has completed its life cycle. */
        };

        /**
         * Destructor.
         */
        virtual ~ThreadObserver() throw() { }

        /**
         * Called by a thread to indicate a change in its state.
         *
         * @param oldState the thread's previous state.
         * @param newState the thread's current state.
         */
        virtual void switchState(State oldState, State newState)
            throw(AssertException, exception) = 0;

        /**
         * Called by an idle thread to ask whether it should start waiting
         * for something to do.
         *
         * @return true if the thread should wait, false if it should terminate.
         */
        virtual bool canWait() throw(AssertException, exception) = 0;
    };


    /**
     * A convenience class that a thread can use to keep track of its current
     * state, and to notify a ThreadObserver of changes in its state.
     *
     * This class is not reference-counted.
     *
     * @see ThreadObserver
     */
    class ThreadState
    {
    public:
        /**
         * Constructs a ThreadState.
         *
         * @param observer the ThreadObserver that should be notified when
         * the thread's state changes.
         */
        ThreadState(ThreadObserver& observer) throw(AssertException, exception);

        /**
         * Notifies the ThreadObserver that the thread's state is
         * ThreadObserver::State::Finished.
         */
        ~ThreadState() throw();

        /**
         * Called by the thread when it isn't doing anything.
         */
        void idle() throw(AssertException, exception);

        /**
         * Called by the thread when it's waiting for something to do.
         */
        void waiting() throw(AssertException, exception);

        /**
         * Called by the thread when it's busy.
         */
        void working() throw(AssertException, exception);

    private:
        ThreadObserver& observer;
        ThreadObserver::State state;

        void switchState(ThreadObserver::State newState) throw(AssertException, exception);
    };
}

#endif /* _NBPP_THREAD_OBSERVER */
