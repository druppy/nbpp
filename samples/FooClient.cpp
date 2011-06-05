#include "FooClient.hpp"
#include <iostream>

FooClient::FooClient(FooCache cache) :
    impl(new FooClientImpl(cache)) { }

FooClient::~FooClient() { }

void FooClient::getFoos()
{
    impl->getFoos();
}

FooClientImpl::FooClientImpl(FooCache arg_cache) : cache(arg_cache)
{
    cout << "FooClientImpl constructed." << endl;
}

FooClientImpl::~FooClientImpl()
{
    cout << "FooClientImpl destructor called." << endl;
}

void FooClientImpl::getFoos()
{
    Foo foo;

    for (int i = 0; i <= cache.getMaxIndex(); ++i)
    {
        foo = cache.getFoo(i);
        if (foo)
        {
            fooList.push_back(foo);
            cout << "FooClientImpl got Foo \"" << foo.getValue() <<
                "\" at index " << i << "." << endl;
        }
    }
}
