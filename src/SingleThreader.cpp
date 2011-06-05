//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/SingleThreader.hpp>
#include <nb++/SingleThreaderImpl.hpp>

namespace nbpp
{
    SingleThreader::SingleThreader() throw(AssertException, exception) :
        ThreaderTemplate<SingleThreaderHandle>
        (SingleThreaderHandle(new SingleThreaderImpl)) { }
}
