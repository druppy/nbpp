//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ThreadPerCommand.hpp>
#include <nb++/ThreadPerCommandImpl.hpp>

namespace nbpp
{
    ThreadPerCommand::ThreadPerCommand() throw(AssertException, exception) :
        ThreaderTemplate<ThreadPerCommandHandle>
        (ThreadPerCommandHandle(new ThreadPerCommandImpl)) { }
}
