//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_EVENT_MULTICASTER
#define _NBPP_EVENT_MULTICASTER

#include <nb++/osdep.hpp>
#include <nb++/Exception.hpp>
#include <nb++/RWLocker.hpp>

#include <list>

namespace nbpp
{
    using std::list;

    /**
     * A template class that maintains a registry of event listeners
     * of some class L.  The sendEvent() method sends an event to all
     * the listeners by invoking some member function on each
     * listener, passing the event as an argument.  An event can be an
     * object of any class.  Given an event class E, the listener
     * class should have one or more member functions that take a
     * single const E& argument and return void, e.g:
     *
     * <pre>
     * void foo(const E&);
     * </pre>
     *
     * This class is not reference-counted; each EventMulticaster
     * object is intended to be a private member variable of some class.
     */
    template<class L> class EventMulticaster
    {
    public:
        EventMulticaster() throw(AssertException, exception) { }
        ~EventMulticaster() throw() { }

        /**
         * Registers an event listener.
         */
        void addListener(L* listener) throw(AssertException, exception)
        {
            WriteLock writeLock(rwLocker);
            listeners.push_back(listener);
        }

        /**
         * Removes an event listener from the registry.
         */
        void removeListener(L* listener) throw(AssertException, exception)
        {
            WriteLock writeLock(rwLocker);
            listeners.remove(listener);
        }

        /**
         * Removes all event listener from the registry.
         */
        void removeAllListeners() throw(AssertException, exception)
        {
            WriteLock writeLock(rwLocker);
            listeners.erase(listeners.begin (), listeners.end ());
        }

        /**
         * Sends an event to all registered listeners, by calling the
         * member function pm on each listener, passing the event as
         * an argument.
         */
        template<class E> void sendEvent(const E& event,
                                         void (L::* pm)(const E&))
            const
        {
            // To avoid thread deadlocks in the case where a listener
            // is calling removeListener() from synchronized code while
            // we're trying to send an event to that listener, we copy
            // the list, then send the event from unsynchronized code.
            ReadLock readLock(rwLocker);
            list<L*> copyOfListeners = listeners;
            readLock.release();

            // We can't use for_each with a standard member function
            // adapter here, because gcc 2.91 gets terribly confused.
            for (typename list<L*>::iterator iter = copyOfListeners.begin();
                 iter != copyOfListeners.end();
                 ++iter)
            {
                ((*iter)->*pm)(event);
            }
        }
    
        void copyTo (EventMulticaster<L>& copy) const throw(AssertException, exception)
        {
            ReadLock readLock(rwLocker);
            copy.listeners = listeners;
        }
    
    private:
        EventMulticaster(const EventMulticaster&);
        EventMulticaster& operator=(const EventMulticaster&);

        mutable RWLocker rwLocker;
        list<L*> listeners;
    };
}

#endif /* _NBPP_EVENT_MULTICASTER */
