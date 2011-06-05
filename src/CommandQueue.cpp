//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/CommandQueue.hpp>

namespace nbpp
{
    CommandQueue::CommandQueue() throw(AssertException, exception) :
        impl(new CommandQueueImpl) { }

    void CommandQueue::push(const Command& command) throw(AssertException, exception)
    {
        impl->push(command);
    }
    
    Command CommandQueue::getNext() throw(AssertException, exception)
    {
        return impl->getNext();
    }

    Command CommandQueue::waitForNext() throw(AssertException, exception)
    {
        return impl->waitForNext();
    }

    bool CommandQueue::empty() const throw(AssertException, exception)
    {
        return impl->empty();
    }

    void CommandQueue::notifyAll() throw(AssertException, exception)
    {
        impl->notifyAll();
    }
}
