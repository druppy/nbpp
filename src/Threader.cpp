//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Threader.hpp>
#include <nb++/ThreaderImpl.hpp>

namespace nbpp
{
    Threader::Threader() throw(exception) { }

    Threader::Threader(ThreaderImpl* arg_impl) throw(AssertException, exception) :
        ThreaderTemplate<ThreaderHandle>(ThreaderHandle(arg_impl)) { }
}
