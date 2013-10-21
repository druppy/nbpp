//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_NETWORK_DAEMON
#define _NBPP_NETWORK_DAEMON

#include <nb++/Log.hpp>
#include <nb++/Daemon.hpp>
#include <nb++/SingleThreader.hpp>
#include <nb++/ServerSocket.hpp>

#include <alloca.h>

namespace nbpp
{
    /**
     * A template connection for address type AddressType.
     * Contains the socket and the source address of this connection.
     *
     * This class is not reference-counted, but its member variables are.
     *
     * @see NetworkDaemon
     */
    template<class AddressType>
        class NetworkConnection
        {
        public:
            /**
             * The Socket on which this connection was received.
             */
            Socket sock;

            /**
             * The address from which this connection was received.
             */
            AddressType address;

            NetworkConnection() throw(AssertException, exception) :
                address(AddressType::allocEmptyAddress()) { }

            NetworkConnection(Socket& arg_sock, const AddressType& arg_address)
                throw(AssertException, exception) :
                sock(arg_sock), address(arg_address) { }
        };

    /**
     * A base class template for Daemons that respond to connections on a socket.  A
     * NetworkDaemon can bind to a port and listen for connections; alternatively, it can
     * be run by inetd.  The constructor detects automatically whether inetd is being
     * used.  The template parameter AddressType should be one of the nb++ network
     * address types; it determines the type of socket that is used for connections.
     *
     * A NetworkDaemon uses a Threader to assign incoming connections to threads or
     * processes.  The type of Threader passed to NetworkDaemon's constructor determines
     * the strategy used.  A SingleThreader causes all requests to be executed in a single
     * thread.  A ForkThreader spawns a new child process for each request.  A
     * ThreadPerCommand spawns a new thread for each request.  A MultiThreader maintains a
     * thread pool that grows and shrinks dynamically according to demand.
     *
     * Derived classes must, at minimum, implement initServerSocket(), handleConnection(),
     * and Daemon::reload().
     *
     * This class is not reference-counted.
     *
     * @see Threader
     */
    template<class AddressType>
        class NetworkDaemon : public Daemon
        {
        public:
            /**
			   Constructs a NetworkDaemon.  Detects whether inetd is being used.

			   @param arg_threader a Threader for assigning incoming connections to
			   threads or processes.
             */
            NetworkDaemon(const string& arg_programName,
                          Threader arg_threader = SingleThreader())
                throw(AssertException, exception) :
                Daemon(arg_programName), threader(arg_threader)
            {
                socklen_t namelen = 0;
                fromInetd = ::getsockname(0, NULL, &namelen) != -1;
            }

            /**
			   Destructor.
             */
            virtual ~NetworkDaemon() throw() { }

            /**
			   If inetd is being used, calls setSignals() and handleConnection().  Otherwise,
			   calls initServerSocket(), then Daemon::run().  If an Exception is caught, it is
			   passed to handleException().

			   @return 0 if called from inetd and the connection is handled successfully, 1 if
			   handleException() is called (regardless of its return value), otherwise the
			   return value of Daemon::run().
             */
            virtual int run() throw() {
                try {
                    if (fromInetd) {
                        NetworkConnection<AddressType> conn;
                        socklen_t sa_addrlen = conn.address.getNativeFormMaxSize();
                        unsigned char *sa_addr = (unsigned char *)alloca( sa_addrlen + 1 );
                        ::getsockname(0, reinterpret_cast<sockaddr*>(sa_addr),
                                      &sa_addrlen);
                        conn.address.assignFromNativeForm
                            (reinterpret_cast<sockaddr*>(sa_addr),
                             sa_addrlen);

                        setSignals();

                        conn.sock = Socket(0, conn.address);
                        handleConnection(conn);
                        return 0;
                    }
                    else
                        serverSock = initServerSocket();

                    threader.init();
                }
                catch( const Exception& e ) {
                    if( !handleException( e ))
						return 1;
                }
                catch( const exception& sysE ) {
                    return 1;
                }

                return Daemon::run();
            }

            /**
			   If this is a standalone daemon, calls DaemonBase::detach().  If inetd is being
			   used, returns 0.

			   @return the return value of DaemonBase::detach() (if this is a standalone
			   daemon), or 0 (if inetd is being used).
             */
            virtual pid_t detach( int fdout = -1, int fderr = -1, int fdin = -1,
                                  bool fd_close = true)
				throw(ThreadControlException, AssertException, exception)
            {
                return fromInetd ? 0 : DaemonBase::detach( fdout, fderr, fdin, fd_close );
            }

            /**
			   @return true if the daemon was called from inetd.
             */
            bool isFromInetd() const throw(AssertException, exception) {
                return fromInetd;
            }

            /**
			   @return if this is a standalone daemon, returns the ServerSocket that it is
			   listening to; if this daemon was called from inetd, returns a null
			   ServerSocket.
             */
            ServerSocket getServerSocket() throw(AssertException, exception) {
                return serverSock;
            }

            /**
			   @return the Threader.
             */
            Threader getThreader() const throw(AssertException, exception) {
                return threader;
            }

            /**
			   Changes the Threader.

			   @return the previous Threader.
             */
            Threader setThreader(Threader arg_threader) throw(AssertException, exception) {
                Threader oldThreader = threader;

                threader = arg_threader;
                if (oldThreader.isInitialized())
                {
                    threader.init();
                }

                return oldThreader;
            }

        protected:
            /**
			   Accepts a connection on the server socket provided by initServerSocket(), and
			   queues the resulting connection for further delivery to handleConnection().
			   The Threader determines when handleConnection is actually called.
             */
            virtual bool mainLoop() throw(Exception, exception) {
				NetworkConnection<AddressType> conn;
				try {
					conn.sock = serverSock.accept(conn.address);

					if( conn.sock ) {
						threader.queue(Command
									   (new OneArgCommandImpl<NetworkConnection<AddressType>,
										NetworkDaemon<AddressType> >
										(conn, this,
										 &NetworkDaemon<AddressType>::handleConnection)));
					}
				} catch( const TimeoutException &ex ) {
					nbpp::log.put( "Ignoring timeout exception." );
				} catch( const exception &ex ) {
					nbpp::log.put( "Ignoring exception : %s", ex.what());
				}

				return true;
			}

            /**
			   Initializes a server socket, which will be used if the daemon is run as a
			   standalone program.  An Exception thrown by this method will be caught and
			   passed to handleException().
             */
            virtual ServerSocket initServerSocket() throw(Exception, exception) = 0;

            /**
			   Called when a connection arrives for the daemon.  If inetd is being used, this
			   method should call SignalManager::deliver() to cause the daemon to be
			   notified synchronously of pending signals.  An Exception thrown by this method
			   will be caught and passed to handleException().

			   @param connection a NetworkConnection<AddressType> containing both the socket
			   and the address from which the connection was received.
             */
            virtual void handleConnection(NetworkConnection<AddressType>&
                                          connection)
                throw(Exception, exception) = 0;

            /**
			   Ignores SIGPIPE to give Socket::waitForInput() and Socket::waitToSend() a
			   chance to throw SocketException on disconnect.
             */
            virtual void setSignals() throw(AssertException, exception)
            {
                Daemon::setSignals();
                theSignalManager().ignoreSignal(SIGPIPE);
            }

        private:
            bool fromInetd;
            ServerSocket serverSock;
            Threader threader;
        };
}

#endif /* _NBPP_NETWORK_DAEMON */
