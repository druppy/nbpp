//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_DAEMON_BASE
#define _NBPP_DAEMON_BASE

#include <nb++/Exception.hpp>
#include <nb++/SignalManager.hpp>

#include <string>

namespace nbpp
{
    using std::string;

    /**
     * A base class for daemons.  Derived classes must, at minimum, implement run() and
     * SignalHandler::handleSignal().
     *
     * This class is not reference-counted.
     */
    class DaemonBase : public SignalHandler
    {
    public:
        /**
         * Constructor.
         *
         * @param programName the name of the program.
         */
        DaemonBase(const string& programName) throw(AssertException, exception);

        /**
         * Unregisters this object as a signal handler.
         */
        virtual ~DaemonBase() throw();

        /**
		   Forks and detaches the daemon process from the controlling terminal.
		   Typically, the parent typically returns from main() after calling this method,
		   and the child calls run().
		 
		   This function will make sure all input and output will be redirected to
		   /dev/null in the child process !
         
		   @param fdout Where to put standart out, -1 is /dev/null
		   @param fderr Where to put standart error, -1 is /dev/null
		   @param fdin Where to get standart in, -1 is /dev/null
           @param fd_close close all non standard file handlers (def. true)
		   @return the child process's pid, or 0 if this process is the child.
		   @exception ThreadControlException if the child process cannot be created.
         */
        virtual pid_t detach( int fdout = -1, int fderr = -1, int fdIn = -1, 
                              bool fd_close = true ) 
			throw(ThreadControlException, AssertException, exception);

        /**
         * Runs the daemon core.  Called by detach().
         *
         * @return 0 on success, non-zero on failure.
         */
        virtual int run() throw() = 0;

    protected:
        /**
         * Registers this object as a synchronous SignalHandler for SIGINT, SIGHUP, and
         * SIGTERM.  This method can be called from the run() method.
         */
        virtual void setSignals() throw(AssertException, exception);

        string programName;
    };
}

#endif /* _NBPP_DAEMON_BASE */
