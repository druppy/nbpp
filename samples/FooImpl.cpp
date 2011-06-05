#include "FooImpl.hpp"

FooImpl::~FooImpl() { }

DefaultFooImpl::~DefaultFooImpl() { }

std::string DefaultFooImpl::getValue()
{
    return "";
}
