//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
    ThreaderImpl::ThreaderImpl() : initialized( false ) { }

    ThreaderImpl::~ThreaderImpl() throw() { }

    void ThreaderImpl::init()
    {
        if (!initialized) {
            initialized = true;
            doInit();
        }
    }

    bool ThreaderImpl::isInitialized() const
    {
        return initialized;
    }
}
