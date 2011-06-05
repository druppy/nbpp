//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/CommandQueueImpl.hpp>

namespace nbpp
{
	void CommandQueueImpl::push(const Command& command) throw(AssertException, exception)
    {
		Lock lock(mutex);
		q.push(command);
		lock.notifyAll();
	}

	Command CommandQueueImpl::getNext(bool block) throw(AssertException, exception)
    {
		Lock lock(mutex);
		Command cmd;

		if (!q.empty())
        {
			cmd = q.front();
			q.pop();
			return cmd;
		}
        else if (block)
        {
			lock.wait();

			if (!q.empty())
            {
				cmd = q.front();
				q.pop();
			}
		}

		return cmd;
	}

	Command CommandQueueImpl::getNext() throw(AssertException, exception)
    {
		return getNext(false);
	}

	Command CommandQueueImpl::waitForNext() throw(AssertException, exception)
    {
		return getNext(true);
	}

	bool CommandQueueImpl::empty() const throw(AssertException, exception)
    {
		Lock lock(mutex);
		return q.empty();
	}

	void CommandQueueImpl::notifyAll() throw(AssertException, exception)
    {
		Lock lock(mutex);
		lock.notifyAll();
	}
}
