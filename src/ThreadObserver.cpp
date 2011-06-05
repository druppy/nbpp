//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ThreadObserver.hpp>

namespace nbpp
{
    ThreadState::ThreadState(ThreadObserver& arg_observer)
        throw(AssertException, exception) :
        observer(arg_observer), state(ThreadObserver::Finished)
    {
        switchState(ThreadObserver::Idle);
    }

    ThreadState::~ThreadState() throw()
    {
        try
        {
            switchState(ThreadObserver::Finished);
        }
        catch (...) { }
    }

    void ThreadState::idle() throw(AssertException, exception)
    {
        switchState(ThreadObserver::Idle);
    }
    void ThreadState::waiting() throw(AssertException, exception)
    {
        switchState(ThreadObserver::Waiting);
    }

    void ThreadState::working() throw(AssertException, exception)
    {
        switchState(ThreadObserver::Working);
    }

    void ThreadState::switchState(ThreadObserver::State newState)
        throw(AssertException, exception)
    {
        if (state != newState)
        {
            observer.switchState(state, newState);
            state = newState;
        }
    }
}
