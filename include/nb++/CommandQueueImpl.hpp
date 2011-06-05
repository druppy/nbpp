//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_COMMAND_QUEUE_IMPL
#define _NBPP_COMMAND_QUEUE_IMPL

#include <nb++/Exception.hpp>
#include <nb++/Command.hpp>
#include <nb++/Mutex.hpp>

#include <queue>

namespace nbpp
{
    using std::queue;

    class CommandQueueImpl : public RefCounted
    {
    public:
        void push(const Command& command) throw(AssertException, exception);

        Command getNext() throw(AssertException, exception);

        Command waitForNext() throw(AssertException, exception);

        bool empty() const throw(AssertException, exception);

        void notifyAll() throw(AssertException, exception);

    private:
        mutable RecursiveMutex mutex;
        queue<Command> q;

        Command getNext(bool block) throw(AssertException, exception);
    };
}


#endif /* _UCCPKIT_COMMAND_QUEUE_IMPL */
