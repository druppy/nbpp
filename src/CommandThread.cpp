//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <iostream>
using namespace std;

#include <nb++/CommandThread.hpp>
#include <nb++/ThreadPool.hpp>

namespace nbpp
{
    CommandThreadImpl::CommandThreadImpl(ThreadObserver& arg_observer,
                                         CommandQueue arg_commandQueue,
                                         unsigned int arg_maxRequests)
        throw(AssertException, exception) :
        thread(*this, false), observer(arg_observer), commandQueue(arg_commandQueue),
        maxRequests(arg_maxRequests), done(false), canceled(false) { }

    CommandThreadImpl::~CommandThreadImpl() throw() { }

    void CommandThreadImpl::start()
        throw(ThreadControlException, AssertException, exception)
    {
        thread.start();
    }

    void CommandThreadImpl::cancel()
        throw(AssertException, exception)
    {
        canceled = true;

        // Notify ourselves if we're waiting on commandQueue.waitForNext().
        commandQueue.notifyAll();
    }

    void CommandThreadImpl::run() throw()
    {
        ThreadState state(observer);

		// cout << "CommandThreadImpl::run() called" << endl;

        try
        {
            // Loop until we're canceled, we exceed our maximum number of
            // requests, or the thread observer doesn't allow us to wait.
            for (unsigned int requestsHandled = 0;
                 !(canceled || (maxRequests && requestsHandled >= maxRequests));
                 ++requestsHandled)
            {

                if (!observer.canWait()) break;
                state.waiting();

                // Wait until we're canceled, or until we get a non-null Command.
                Command cmd;
                while (!(canceled || (cmd = commandQueue.waitForNext()) == true)) { }
		// cout << "CommandThreadImpl::run(): got a Command" << endl;

                if (canceled) break;

                state.working();
                cmd.execute();
		// cerr << "CommandThreadImpl::run(): executed the Command" << endl;
                state.idle();
            }
        } catch (...) { }

        done = true;
    }

    bool CommandThreadImpl::isDone() const throw(AssertException, exception)
    {
        return done;
    }

    bool CommandThreadImpl::wasCanceled() const throw(AssertException, exception)
    {
        return canceled;
    }

    CommandThread::CommandThread(ThreadObserver& observer, CommandQueue commandQueue,
                                 unsigned int maxRequests)
        throw(AssertException, exception) :
        impl(new CommandThreadImpl(observer, commandQueue, maxRequests)) { }
    
    void CommandThread::start()
        throw(ThreadControlException, AssertException, exception)
    {
        impl->start();
    }
    
    void CommandThread::cancel()
        throw(AssertException, exception)
    {
        impl->cancel();
    }
    
    bool CommandThread::isDone() const throw(AssertException, exception)
    {
        return impl->isDone();
    }
    
    bool CommandThread::wasCanceled() const throw(AssertException, exception)
    {
        return impl->wasCanceled();
    }
}
