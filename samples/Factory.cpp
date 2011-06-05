#include "Factory.hpp"

Foo Factory::makeFoo(const string& value)
{
    return Foo(new SubFooImpl(value));
}

FooCache Factory::makeFooCache()
{
    return FooCache();
}

FooClient Factory::makeFooClient(FooCache cache)
{
    return FooClient(cache);
}
