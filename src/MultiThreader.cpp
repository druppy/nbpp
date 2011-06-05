//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/MultiThreader.hpp>
#include <nb++/MultiThreaderImpl.hpp>

namespace nbpp
{
    MultiThreader::MultiThreader() throw(AssertException, exception) :
        ThreaderTemplate<MultiThreaderHandle>
        (MultiThreaderHandle(new MultiThreaderImpl)) { }

    ThreadPool& MultiThreader::getThreadPool() throw(AssertException, exception)
    {
        return impl->getThreadPool();
    }
}
