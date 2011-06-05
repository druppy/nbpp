//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include "nb++/Daemon.hpp"

extern "C"
{
#include <signal.h>
}

namespace nbpp
{
    Daemon::Daemon(const string& arg_programName) throw(AssertException, exception) :
        DaemonBase(arg_programName), fShutdown(false), fReload(false) { }

    Daemon::~Daemon() throw() { }

    int Daemon::run() throw()
    {
        try {
            try {
                setSignals();
            } catch( const Exception& e ) {
                if (!handleException(e))
                    return 1;
            }
            
            while (!fShutdown) {
                try {
                    if(fReload) {
                        fReload = false;
                        reload();
                    }
                    if (!mainLoop())
                        break;
                } catch( const Exception& e ) {
                    if (!handleException(e))
                        return 1;
                } catch( const std::exception& sysE) {
					if(!handleSystemException( sysE ))
						return 1;
				}

                try {
                    theSignalManager().deliver();
                } catch( const Exception& e ) {
                    if (!handleException(e))
                        return 1;
                }
            }
        } catch (const std::exception& sysE) {
			return 1;
        }

        return 0;
    }

    void Daemon::handleSignal(const SignalEvent& event) throw()
    {
        try {
            switch(event.getSigNum()) {
            case SIGHUP:
                fReload = true;
                break;
            case SIGTERM:
                fShutdown = true;
                break;
            default:
                break;
            }
        } catch(...) { }
    }

    bool Daemon::handleException(const Exception& e) throw(exception)
    {
        return false;
    }

    bool Daemon::handleSystemException(const std::exception& e) throw()
    {
        return false;
    }

}
