//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/CommandQueueImpl.hpp>
#include <iostream>

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
		} else if (block) {
            try {
    			lock.wait( 10 ); // timeout after 10 sec ... one never knowns

    			if (!q.empty())
                {
    				cmd = q.front();
    				q.pop();
    			}
            } catch( const AssertException &ex ) {
                std::clog << "ERROR: " << __FILE__ << ":" << __LINE__ << ", say " << ex.what() << std::endl;
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
