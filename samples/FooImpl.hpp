#ifndef _FOO_IMPL
#define _FOO_IMPL

#include <osdep.hpp>
#include <RefCount.hpp>

/*
 * A trivial interface.
 */
class FooImpl : public nbpp::RefCounted  
{
public:
    virtual ~FooImpl() = 0;
    virtual std::string getValue()= 0;
};

/*
 * A default implementation.
 */
class DefaultFooImpl : public FooImpl
{
public:
    virtual ~DefaultFooImpl(); 
    virtual std::string getValue();
};

#endif /* _FOO_IMPL */
