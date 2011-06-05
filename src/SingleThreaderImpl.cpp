//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/SingleThreaderImpl.hpp>

namespace nbpp
{
    void SingleThreaderImpl::queue(Command command)
        throw(ThreadControlException, AssertException, exception)
    {
        try
        {
            command.execute();
        }
        catch (exception)
        {
            throw;
        }
        catch (...) { }
    }

    void SingleThreaderImpl::doInit()
        throw(ThreadControlException, AssertException, exception) { }
}
