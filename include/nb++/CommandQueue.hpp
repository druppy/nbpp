//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_COMMAND_QUEUE
#define _NBPP_COMMAND_QUEUE

#include <nb++/CommandQueueImpl.hpp>

namespace nbpp
{
    /**
     * A thread-safe queue of Command objects.
     *
     * This class is reference-counted.
     */
    class CommandQueue
    {
    public:
        /**
         * Constructs a CommandQueue.
         */
        CommandQueue() throw(AssertException, exception);

        /**
         * Adds a Command to the queue.  Wakes up any threads that are blocked
         * on waitForNext().
         */
        void push(const Command& command) throw(AssertException, exception);

        /**
         * @return the next Command on the queue, or a null Command if the queue
         * is empty.
         */
        Command getNext() throw(AssertException, exception);

        /**
         * If the queue is non-empty, returns the next Command on the queue.  If
         * the queue is empty, blocks until a Command is added to the queue.
         *
         * @return the next available Command, or a null Command if a Command was
         * added but another thread got it.
         */
        Command waitForNext() throw(AssertException, exception);

        /**
         * @return true if the queue is empty.
         */
        bool empty() const throw(AssertException, exception);

        /**
         * Wakes up any threads that are blocked on waitForNext().
         */
        void notifyAll() throw(AssertException, exception);

    private:
        RefHandle<CommandQueueImpl> impl;
    };
}


#endif /* _NBPP_COMMAND_QUEUE */
