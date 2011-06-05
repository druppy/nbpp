#ifndef _FOO_CLIENT
#define _FOO_CLIENT

#include <osdep.hpp>
#include <list>
#include "Foo.hpp"
#include "FooCache.hpp"

class FooClientImpl;

/**
 * Gets whatever Foos are available from a FooCache, and
 * reports on them.
 */
class FooClient
{
    RefHandle<FooClientImpl> impl;

public:
    FooClient(FooCache cache);
    ~FooClient();

    void getFoos();
};

class FooClientImpl : public nbpp::RefCounted
{
    FooCache cache;
    list<Foo> fooList;

public:
    FooClientImpl(FooCache cache);
    virtual ~FooClientImpl();

    virtual void getFoos();
};

#endif /* _FOO_CLIENT */
