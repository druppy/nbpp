//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Command.hpp>

namespace nbpp
{
    CommandImpl::~CommandImpl() throw() { }

    Command::Command(CommandImpl* arg_impl) throw(AssertException, exception) :
        impl(arg_impl) { }

    void Command::execute()
    {
        impl->execute();
    }

    Command::Command() throw(exception) { }

    Command::operator bool() const throw()
    {
        return impl;
    }
}
