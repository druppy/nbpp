//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ForkThreader.hpp>
#include <nb++/ForkThreaderImpl.hpp>

namespace nbpp
{
    ForkThreader::ForkThreader() throw(AssertException, exception) :
        ThreaderTemplate<ForkThreaderHandle>
        (ForkThreaderHandle(new ForkThreaderImpl)) { }
}
