#ifndef _FOO
#define _FOO

#include "FooImpl.hpp"

using namespace std;
using namespace nbpp;

/*
 * A reference-counted wrapper for an object that has a string
 * value.  This is the interface for clients.
 */
class Foo
{
    RefHandle<FooImpl> impl;

public:
    Foo();
    Foo(FooImpl* realImpl);
    ~Foo();

    string getValue();
    operator bool() const;
};

#endif /* _FOO */
