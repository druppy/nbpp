//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_DAEMON
#define _NBPP_DAEMON

#include <nb++/DaemonBase.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    /**
     * A base class for daemons that run in an infinite loop, and that need to handle
     * SIGHUP, SIGTERM, and SIGINT in the usual way.  The run() method calls mainLoop()
     * repeatedly.  When SIGHUP is received, a flag is set that causes reload() to be
     * called on the next iteration.  When SIGTERM is received, a flag is set that causes
     * run() to return on the next iteration.
     *
     * Derived classes must, at minimum, implement mainLoop() and reload().
     *
     * This class is not reference-counted.
     */
    class Daemon : public DaemonBase
    {
    public:
        /**
         * Constructor.
         *
         * @param programName the name of the program.
         */
        Daemon (const string& programName) throw(AssertException, exception);

        /**
         * Destructor.
         */
        virtual ~Daemon() throw();

        /**
         * Calls DaemonBase::setSignals().  Then calls mainLoop() repeatedly until
         * fShutdown becomes true.  Before each call to mainLoop(), calls reload() if
         * fReload is true.  Calls theSignalManager().deliver() after each call to
         * mainLoop().
         *
         * Catches Exceptions, and passes them to handleException().  If handleException()
         * returns true, the loop continues; otherwise run() returns 1.
         *
         * @return 0, unless a call to handleException() returns false, in which case
         * returns 1.
         */
        virtual int run() throw();

        /**
         * When SIGHUP is received, sets fReload to true, causing run() to call reload()
         * on its next iteration.  When SIGTERM is received, sets fShutdown to true,
         * causing run() to return on its next iteration.
         */
        virtual void handleSignal(const SignalEvent& event) throw();

    protected:
        /**
         * Called repeatedly from run() until fShutdown is true, i.e. until SIGTERM is
         * received.
         *
         * @exception Exception if an error occurs.
         */
        virtual bool mainLoop() throw(Exception, exception) = 0;

        /**
         * Called when fReload is true, i.e. when SIGHUP is received.
         *
         * @exception Exception if an error occurs.
         */
        virtual void reload() throw(Exception, exception) = 0;

        /**
		   Called by run() when it catches an Exception.  The default implementation
		   always returns false.
         
		   @param e The nb++ exception arrived
		   @return true if the exception was handled successfully, false if the program
		   should terminate.
         */
        virtual bool handleException( const Exception& e) throw(exception);

		/**
		   Handle standard C++ exceptions, if any arrive.

		   @param e The standard exception arrived
		 */
        virtual bool handleSystemException( const std::exception& e) throw();

        bool fShutdown;
        bool fReload;
    };
}

#endif /* _NBPP_DAEMON */
