#ifndef _SUBFOO_IMPL
#define _SUBFOO_IMPL

#include "Foo.hpp"

/**
 * A wrapper for a string.
 */
class SubFooImpl : public FooImpl
{
    string value;

public:
    SubFooImpl(const string& value = "");
    virtual ~SubFooImpl();
    virtual string getValue();
};

#endif /* _SUBFOO_IMPL */
