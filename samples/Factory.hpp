#ifndef _FACTORY
#define _FACTORY

#include "SubFooImpl.hpp"
#include "FooClient.hpp"

/*
 * A factory class that produces Foo, FooCache, and FooClient
 * objects.  The Foo objects use the SubFooImpl implementation of the
 * FooImpl interface.
 */
class Factory
{
public:
    static Foo makeFoo(const string& value);

    static FooCache makeFooCache();

    static FooClient makeFooClient(FooCache cache);
};

#endif /* _FACTORY */
