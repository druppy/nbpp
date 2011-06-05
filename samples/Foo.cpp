#include "Foo.hpp"

Foo::Foo() { }

Foo::Foo(FooImpl* realImpl) : impl(realImpl) { }

Foo::~Foo() { }

string Foo::getValue()
{
    return impl->getValue();
}

Foo::operator bool() const
{
    return impl;
}
