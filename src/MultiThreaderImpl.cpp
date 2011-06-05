//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/MultiThreaderImpl.hpp>

namespace nbpp
{
    MultiThreaderImpl::~MultiThreaderImpl() throw() { }

    void MultiThreaderImpl::queue(Command command)
        throw(ThreadControlException, AssertException, exception)
    {
        pool.queue(command);
    }

    ThreadPool& MultiThreaderImpl::getThreadPool() throw(AssertException, exception)
    {
        return pool;
    }
        
    void MultiThreaderImpl::doInit()
        throw(ThreadControlException, AssertException, exception)
    {
        pool.init();
    }
}
