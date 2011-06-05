//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ForkThreaderImpl.hpp>
#include <nb++/SignalManager.hpp>

extern "C"
{
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
}

namespace nbpp
{
    ForkThreaderImpl::~ForkThreaderImpl() throw()
    {
        try
        {
            if (isInitialized())
            {
                theSignalManager().defaultSignal(SIGCHLD);
            }
        }
        catch (...) { }
    }

    void ForkThreaderImpl::queue(Command command)
        throw(ThreadControlException, AssertException, exception)
    {
        switch (::fork ())
        {
        case 0:
            try
            {
                command.execute();
            }
            catch (...) { }
            exit (0);
        case -1:
            throw ThreadControlException(strerror_r(errno));
        default:
            break;
        }
    }

    void ForkThreaderImpl::doInit()
        throw(ThreadControlException, AssertException, exception)
    {
        theSignalManager().ignoreSignal(SIGCHLD);
    }
}
