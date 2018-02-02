//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ServerSocketImpl.hpp>

extern "C"
{
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
}

namespace nbpp
{
    ServerSocketImpl::ServerSocketImpl(NetworkAddress networkAddress, int arg_backlog)
        throw(IOException, AssertException, exception) :
        SocketBaseImpl(networkAddress),
        backlog(arg_backlog)
    {
        init();
    }

    void ServerSocketImpl::init() throw(IOException, AssertException, exception)
    {
        // Let the kernel reuse the socket without waiting for the
        // port to time out.
        int reuse = 1;
        setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

		// Timeout after 10 sec if not closed right
		struct linger lin;
		lin.l_onoff = 1;
		lin.l_linger = 10;
		setsockopt(m_fd, SOL_SOCKET, SO_LINGER, &lin, sizeof( struct linger ));

        // Bind the socket to the port.
        if (bind(m_fd, address.getNativeForm(), address.getNativeFormSize()) == -1) {
            close();
            throw BindException(errno);
        }

        // Listen for connections.
        if (listen(m_fd, backlog) == -1) {
			close();
            throw IOException(errno);
        }
    }

    Socket ServerSocketImpl::accept() const throw(IOException, AssertException, exception)
    {
        return accept(NetworkAddress(address.makeEmptyClone()));
    }

    Socket ServerSocketImpl::accept(NetworkAddress clientAddress) const
        throw(IOException, AssertException, exception)
    {
        while (wait(m_fd, true, false)) {
            int conn;
            socklen_t len = clientAddress.getNativeFormMaxSize();
            unsigned char clientAddrBuf [len];
            while((conn = ::accept(m_fd, reinterpret_cast<sockaddr*>(clientAddrBuf), &len)) < 0) {
                if( errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;

                throw ConnectException(errno);
            }

            if (clientAddress)
                clientAddress.assignFromNativeForm(
					reinterpret_cast<sockaddr*>(clientAddrBuf), len);

			// Timeout after 10 sec if not closed right
			struct linger lin;
			lin.l_onoff = 1;
			lin.l_linger = 10;
			setsockopt(conn, SOL_SOCKET, SO_LINGER, &lin, sizeof( struct linger ));

            return Socket(conn, clientAddress);
        }
        
        throw TimeoutException( "Timed out waiting for a connection" );
    }
}
