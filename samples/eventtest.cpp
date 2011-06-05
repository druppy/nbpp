#include <iostream>
#include <EventMulticaster.hpp>
#include <ExceptionGuard.hpp>

/*
 * Demonstrates EventMulticaster.  An imaginary washer-dryer sends
 * LaundryEvents to several objects that implement an interface called
 * LaundryListener.  These objects are LaundryLoggers, which log the
 * washer-dryer's progress to standard out.
 */

using namespace std;
using namespace nbpp;

class WasherDryer;
class LaundryEvent;

/*
 * An interface for objects that handle LaundryEvents.
 */
class LaundryListener
{
public:
    virtual ~LaundryListener() { }
    virtual void washBegun(const LaundryEvent& event) = 0;
    virtual void washEnded(const LaundryEvent& event) = 0;
    virtual void dryBegun(const LaundryEvent& event) = 0;
    virtual void dryEnded(const LaundryEvent& event) = 0;
};

/*
 * An event concerning the progress of a WasherDryer.
 */
class LaundryEvent
{
public:
    LaundryEvent() : temperatureGiven(false), temperature(0) { }

    LaundryEvent(int arg_temperature) :
        temperatureGiven(true), temperature(arg_temperature) { }

    bool hasTemperature() const
    {
        return temperatureGiven;
    }

    int getTemperature() const
    {
        return temperature;
    }

private:
    bool temperatureGiven;
    int temperature;
};


/*
 * A washer-dryer that tells its LaundryListeners what it's doing.
 */
class WasherDryer
{
public:
    WasherDryer() { }

    void addLaundryListener(LaundryListener* listener)
    {
        multicaster.addListener(listener);
    }

    void removeLaundryListener(LaundryListener* listener)
    {
        multicaster.removeListener(listener);
    }

    void wash()
    {
        multicaster.sendEvent(LaundryEvent(25), &LaundryListener::washBegun);
        multicaster.sendEvent(LaundryEvent(), &LaundryListener::washEnded);
    }

    void dry()
    {
        multicaster.sendEvent(LaundryEvent(40), &LaundryListener::dryBegun);
        multicaster.sendEvent(LaundryEvent(), &LaundryListener::dryEnded);
    }

private:
    EventMulticaster<LaundryListener> multicaster;
};


/*
 * A LaundryListener that logs information about the LaundryEvents
 * it receives from a WasherDryer.
 */
class LaundryLogger : public LaundryListener
{
public:
    LaundryLogger(int arg_id, WasherDryer& arg_washerDryer) :
        id(arg_id), washerDryer(arg_washerDryer)
    {
        washerDryer.addLaundryListener(this);
    }

    ~LaundryLogger()
    {
        washerDryer.removeLaundryListener(this);
    }
    
    virtual void washBegun(const LaundryEvent& event)
    {
        logEvent(event, "wash begun");
    }

    virtual void washEnded(const LaundryEvent& event)
    {
        logEvent(event, "wash ended");
    }

    virtual void dryBegun(const LaundryEvent& event)
    {
        logEvent(event, "dry begun");
    }

    virtual void dryEnded(const LaundryEvent& event)
    {
        logEvent(event, "dry ended");
    }

private:
    void logEvent(const LaundryEvent& event, const string& message)
    {
        cout << "LaundryLogger " << id << ": " << message;
        if (event.hasTemperature())
        {
            cout << " at " << event.getTemperature() << " degrees";
        }
        cout << endl;
    }

    int id;
    WasherDryer& washerDryer;
};


/*
 * Does the laundry.  Three LaundryLoggers log the progress of the WasherDryer.
 */
int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        WasherDryer washerDryer;

        LaundryLogger logger1(1, washerDryer);
        LaundryLogger logger2(2, washerDryer);
        LaundryLogger logger3(3, washerDryer);

        washerDryer.wash();
        washerDryer.dry();
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
