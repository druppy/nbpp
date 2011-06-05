//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SIGNAL_MANAGER
#define _NBPP_SIGNAL_MANAGER

#include <nb++/Exception.hpp>
#include <nb++/EventMulticaster.hpp>
#include <nb++/RefCount.hpp>

#include <map>

extern "C"
{
#include <signal.h>
}

namespace nbpp
{
    using std::map;

    /**
     * A signal event.  Contains all data received by the signal handler.
     *
     * This class is not reference-counted.
     *
     * @see SignalManager
     * @see SignalHandler
     */
    class SignalEvent : public RefCounted
    {
    public:
        SignalEvent(int signum) throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~SignalEvent() throw();

        /**
         * @return the signal number for this SignalEvent.
         */
        int getSigNum() const throw(AssertException, exception);

    private:
        int signum;
    };
    typedef RefHandle<SignalEvent> SignalEventHandle;

    /**
     * An interface for objects that can be registered with a SignalManager as signal
     * handlers.
     *
     * @see SignalManager
     * @see SignalEvent
     */
    class SignalHandler
    {
    public:
		virtual ~SignalHandler() {}
		
        /**
         * If this SignalHandler has been registered to handle a signal asynchronously,
         * this method is called when the signal arrives.  If the SignalHandler is
         * registered to handle a signal synchronously, this method is called when
         * SignalManager::deliver() is called, if the signal is pending.
         */
        virtual void handleSignal(const SignalEvent& event) throw() = 0;
    };

    /**
     * A singleton that catches signals and dispatches them to objects that implement the
     * SignalHandler interface.  SignalManager is intended to make it convenient to have
     * signals handled at predictable times by methods that aren't async-safe.  The
     * recommended approach is to register handlers using the addSyncHandler() method, and
     * to call the deliver() method from time to time.  When deliver() is called, pending
     * signals are delivered synchronously.
     *
     * It is also possible to register asynchronous handlers; don't do this unless you're
     * sure you know what you're doing.  Each signal can have multiple synchronous
     * handlers, but only one asynchronous handler.
     *
     * SignalManager also provides methods for blocking signals by setting the calling
     * thread's signal mask.  If you only use synchronous handlers, you probably won't
     * need to do this.  The effect of signal masks in multithreaded programs depends
     * on the operating system's implementation.
     *
     * Most of the methods in this class cannot be made thread-safe.  Except for
     * blockSignal() and unblockSignal(), all SignalManager methods should be called by
     * only one thread.
     *
     * This class is not reference-counted.  We suggest that you use the function
     * theSignalManager() to refer to the singleton instance, e.g.:
     *
     * <pre>
     * theSignalManager().deliver();
     * </pre>
     *
     * @see SignalHandler
     * @see SignalEvent
     */
    class SignalManager
    {
    public:
        /**
         * Sets the asynchronous signal handler for a signal.  The handler must be
         * async-safe; use with caution.  Only one asynchronous signal handler can be
         * used for each signal.
         */
        void setAsyncHandler(int signum, SignalHandler* handler)
            throw(AssertException, exception);

        /**
         * Causes a signal to be ignored.  No handlers will be called for the
         * signal.
         */
        void ignoreSignal(int signum) throw(AssertException, exception);

        /**
         * Resets the handling of a signal to the operating system's default.  No
         * handlers will be called for the signal.
         */
        void defaultSignal(int signum) throw(AssertException, exception);

        /**
         * Blocks a signal.  No handlers will be called for the signal until after it is
         * unblocked.
         */
        void blockSignal(int signum) throw(AssertException, exception);

        /**
         * Unblocks a signal.  If the signal has not been ignored and is pending, it is
         * delivered immediately to its asynchronous handler, if it has one, and the next
         * call to deliver() will deliver it to any synchronous handlers.
         */
        void unblockSignal(int signum) throw(AssertException, exception);

        /**
         * Registers a synchronous handler for the specified signal.
         */
        void addSyncHandler(int signum, SignalHandler* handler)
            throw(AssertException, exception);

        /**
         * Unregisters a synchronous handler for the specified signal.
         */
        void removeSyncHandler(int signum, SignalHandler* handler)
            throw(AssertException, exception);

        /**
         * Unregisters a synchronous handler for all signals.
         */
        void removeSyncHandler(SignalHandler* handler)
            throw(AssertException, exception);

        /**
         * Unregisters all synchronous handlers for the specified signal.
         */
        void removeAllSyncHandlers(int signum) throw(AssertException, exception);

        /**
         * Delivers pending synchronous signals.
         */
        void deliver() throw(AssertException, exception);
        
    private:
        struct SignalHandlerData
        {
            SignalHandler* asyncHandler;
            SignalEventHandle event;
            bool pending;
            EventMulticaster<SignalHandler> syncHandlers;
                
            SignalHandlerData() throw(AssertException, exception);

            SignalHandlerData(const SignalHandlerData& right)
                throw(AssertException, exception);
        };

        SignalManager() throw(AssertException, exception);
        SignalManager(const SignalManager&);
        SignalManager& operator=(const SignalManager&);
        friend SignalManager& theSignalManager() throw(AssertException, exception);

        // The function passed to sigaction() as the handler for all signals.
        static NBPP_RETSIGTYPE realHandler(int signum) throw();

        // Sets a signal handler function.
        void setSignal(int signum, NBPP_RETSIGTYPE (*func)(int signum))
            throw(AssertException, exception);

        bool pendingSignals;
        map<int, SignalHandlerData> handlerMap;
    };

    /**
     * Use this function to refer to the SignalManager singleton, e.g.:
     *
     * <pre>
     * theSignalManager().deliver();
     * </pre>
     *
     * This function is not thread-safe.
     */
    SignalManager& theSignalManager() throw(AssertException, exception);
}

#endif /* _NBPP_SIGNAL_MANAGER */
