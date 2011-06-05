#include <iostream>
#include "Factory.hpp"
#include <ExceptionGuard.hpp>

/*
 * A test of Handle.
 *
 * Uses a factory class to make a FooClient, a FooCache, and some Foo
 * objects, all of which are proxies for Handles.  (The factory
 * produces Foo objects that use the SubFooImpl implementation of the
 * FooImpl interface.)  Adds the Foos to the cache, then tells the
 * client to get them from the cache; client and cache then have
 * Handles pointing to the same Foos.  Everything then goes out of
 * scope and is garbage-collected.
 */

int main(int argc, char* argv[])
{
	using std::cerr;

    ExceptionGuard exceptionGuard;

    try
    {
        FooCache cache = Factory::makeFooCache();
        FooClient client = Factory::makeFooClient(cache);

        cache.addFoo(12, Factory::makeFoo("apples"));
        cache.addFoo(3, Factory::makeFoo("pears"));
        cache.addFoo(27, Factory::makeFoo("oranges"));
        cache.addFoo(4, Factory::makeFoo("peaches"));

        client.getFoos();
    }
    catch (Exception& e)
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
