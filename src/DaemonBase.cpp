//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Exception.hpp>
#include <nb++/DaemonBase.hpp>

extern "C"
{
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef NBPP_HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif  // NBPP_HAVE_SYS_RESOURCE_H

	void child_init( void ) {
		std::cout << "fork child, hello" << std::endl;
	}
}



namespace nbpp
{
    DaemonBase::DaemonBase(const string& arg_programName)
        throw(AssertException, exception)
    {
        string::size_type slash = arg_programName.find_last_of ('/');

        programName = (slash == string::npos) ? arg_programName
            : arg_programName.substr (slash + 1);
    }

    DaemonBase::~DaemonBase() throw()
    {
        try
        {
            theSignalManager().removeSyncHandler(this);
        }
        catch (...) { }
    }

    pid_t DaemonBase::detach( int fdout, int fderr, int fdin, bool fd_close ) 
		throw(ThreadControlException, AssertException, exception)
    {
		// Store I/O
		int fd_in = dup( 0 );
		int fd_out = dup( 1 );
		int fd_err = dup( 2 );

		int fd_null = open( "/dev/null", O_RDWR );
		
		dup2( fdin < 0 ? fd_null : fdin, 0 );
		dup2( fdout < 0 ? fd_null : fdout, 1 );
		dup2( fderr < 0 ? fd_null : fderr, 2 );

        pid_t pid = ::fork ();

		close( fd_null );

        switch ( pid )
        {
        case 0: /* child */
            break;
        case -1:
            throw ThreadControlException("DaemonBase::detach(): fork failed");
        default:
			dup2( fd_in,  0 );  // Restore, when parent
			dup2( fd_out, 1 );
			dup2( fd_err, 2 );
            return pid;
        }

#ifdef OPEN_MAX
        int i = OPEN_MAX;
#else   // !OPEN_MAX
#ifdef FOPEN_MAX
        int i = FOPEN_MAX;
#else   // !FOPEN_MAX
        int i = 4096;
#endif  // !FOPEN_MAX
#endif  // !OPEN_MAX
#ifdef NBPP_HAVE_GETRLIMIT
        struct rlimit rl;
        if (::getrlimit (RLIMIT_NOFILE, &rl) != -1)
            i = rl.rlim_max;
#endif  // NBPP_HAVE_GETRLIMIT
        if( fd_close )
            while (2 < i--)
                ::close ( i );
#ifdef NBPP_HAVE_SETSID
        ::setsid ();
#else // !NBPP_HAVE_SETSID
#ifdef NBPP_HAVE_SETPGRP
        ::setpgrp ();
#endif  // NBPP_HAVE_SETPGRP
#endif  // NBPP_HAVE_SETSID
        
        return 0;
    }

    void DaemonBase::setSignals() throw(AssertException, exception)
    {
        theSignalManager().addSyncHandler(SIGINT,  this);
        theSignalManager().addSyncHandler(SIGHUP,  this);
        theSignalManager().addSyncHandler(SIGTERM, this);
    }
}
