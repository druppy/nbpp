#include <osdep.hpp>
#include <iostream>
#include <RefCount.hpp>
#include <ExceptionGuard.hpp>
#include <map>

/*
 * An example of what not to do with Handles.  Creates a pair of
 * objects that point to each other via Handles, and therefore cannot
 * be garbage-collected.  The Handles are stored in STL containers.
 */

using namespace std;
using namespace nbpp;

class Thing;
typedef RefHandle<Thing> ThingHandle;

class Thing : public RefCounted
{
public:
    Thing()
    {
        cout << "Thing constructed." << endl;
    }

    ~Thing()
    {
        cout << "Thing destructor called." << endl;
    }       

    void addThing(string name, ThingHandle tp)
    {
        thingMap[name] = tp;
    }

private:
    map<string, ThingHandle> thingMap;
};

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        ThingHandle thing1 = ThingHandle(new Thing());
        ThingHandle thing2 = ThingHandle(new Thing());
        
        thing1->addThing("thing2", thing2);
        thing2->addThing("thing1", thing1);
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
