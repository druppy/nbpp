#include "FooCache.hpp"
#include <iostream>

FooCache::FooCache() : impl(new FooCacheImpl()) { }

FooCache::~FooCache() { }

void FooCache::addFoo(int index, Foo foo)
{
    impl->addFoo(index, foo);
}

int FooCache::getMaxIndex()
{
    return impl->getMaxIndex();
}

Foo FooCache::getFoo(int index)
{
    return impl->getFoo(index);
}

FooCacheImpl::FooCacheImpl() : maxIndex(0)
{
    cout << "FooCacheImpl constructed." << endl;
}

FooCacheImpl::~FooCacheImpl()
{
    cout << "FooCacheImpl destructor called." << endl;
}

void FooCacheImpl::addFoo(int index, Foo foo)
{
    fooMap[index] = foo;
    if (index > maxIndex)
    {
        maxIndex = index;
    }
}

int FooCacheImpl::getMaxIndex()
{
    return maxIndex;
}

Foo FooCacheImpl::getFoo(int index)
{
    map<int, Foo>::const_iterator iter = fooMap.find(index);
    if (iter != fooMap.end())
    {
        return iter->second;
    }
    else
    {
        return Foo();
    }
}
