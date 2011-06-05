#ifndef _FOO_CACHE
#define _FOO_CACHE

#include <osdep.hpp>
#include <map>
#include "Foo.hpp"

class FooCacheImpl;

/**
 * Maintains a cache of Foo objects mapped to integer
 * indexes.
 */
class FooCache
{
    RefHandle<FooCacheImpl> impl;

public:
    FooCache();
    ~FooCache();

    void addFoo(int index, Foo foo);
    int getMaxIndex();
    Foo getFoo(int index);

private:
};

class FooCacheImpl : public RefCounted
{
    map<int, Foo> fooMap;
    int maxIndex;

public:
    FooCacheImpl();
    virtual ~FooCacheImpl();

    virtual void addFoo(int index, Foo foo);
    virtual int getMaxIndex();
    virtual Foo getFoo(int index);
};

#endif /* _FOO_CACHE */
