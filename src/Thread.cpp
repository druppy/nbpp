//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Thread.hpp>

namespace nbpp
{
    Thread::Thread() throw(exception) { }

    Thread::Thread(Runnable& runnable, bool isJoinable) throw(AssertException, exception) :
        impl(new ThreadImpl(runnable, isJoinable)) { }

    Thread::~Thread() throw() { }

    void Thread::start() throw(ThreadControlException, AssertException, exception)
    {
        impl->start();
    }

    void Thread::detach() throw(ThreadControlException, AssertException, exception)
    {
        impl->detach();
    }

    bool Thread::isJoinable() throw(ThreadControlException, AssertException, exception)
    {
        return impl->isJoinable();
    }

    void Thread::join() throw(ThreadControlException, AssertException, exception)
    {
        impl->join();
    }

    Thread::operator bool() const throw()
    {
        return impl;
    }
}
