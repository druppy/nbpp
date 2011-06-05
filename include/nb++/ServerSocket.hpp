//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SERVER_SOCKET
#define _NBPP_SERVER_SOCKET

#include <nb++/ServerSocketImpl.hpp>

namespace nbpp
{
    /**
     * Represents a server socket.
     *
     * This class is reference-counted.
     */
    class ServerSocket
    {
    public:
        /**
         * Creates a socket and binds it to the specified address and
         * port, with the specified backlog of pending connections;
         * then begins listening for connections.
         *
         * @exception BindException if the port cannot be bound.
         * @exception IOException if some other I/O error occurs.
         */
        explicit ServerSocket(NetworkAddress networkAddress, int backlog = 5)
            throw(IOException, AssertException, exception);

        /**
         * Waits for a connection to be made and accepts it.  If a timeout
         * has been set, the method blocks at most that much time.
         *
         * @exception TimeoutException if the operation times out.
         * @exception ConnectException if an error occurs while
         * accepting a connection.
         */
        Socket accept() const throw(IOException, AssertException, exception);

        /**
         * Waits for a connection to be made and accepts it.  If a timeout
         * has been set, the method blocks at most that much time.
         *
         * @param clientAddress an empty network address object,
         * created with the static allocEmptyAddress() method of one
         * of the specialized types of network address.  The address
         * object's contents will be filled in by this method.
         *
         * @exception TimeoutException if the operation times out.
         * @exception ConnectException if an error occurs while
         * accepting a connection.
         */
        Socket accept(NetworkAddress clientAddress) const
            throw(IOException, AssertException, exception);

        /**
         * @return the file descriptor of the socket
         */
        const int getFd() const throw(AssertException, exception); 

        /**
         * @return the local address to which this socket is connected.
         */
        NetworkAddress getLocalAddress(NetworkAddress addr) const
            throw(IOException, AssertException, exception);

        /**
         * @return the local address to which this socket is connected.
         */
        NetworkAddress getLocalAddress() const
            throw(IOException, AssertException, exception);

        /**
         * @return the remote peer address to which this socket is connected.
         */
        NetworkAddress getPeerAddress(NetworkAddress addr) const
            throw(IOException, AssertException, exception);

        /**
         * @return the remote peer address to which this socket is connected.
         */
        NetworkAddress getPeerAddress() const
            throw(IOException, AssertException, exception);

        /**
         * Closes the socket.  This is done automatically by the
         * destructor.
         */
        void close() throw(AssertException, exception);

        /**
         * Enables/disables a timeout, in seconds, for the accept()
         * method.  A value of 0 means an infinitely long timeout.
         */
        void setTimeout(int sec) throw(AssertException, exception);

        /**
         * @return the timeout, in seconds, that has been set for this
         * socket.  A value of 0 means an infinitely long timeout.
         */
        int getTimeout() const throw(AssertException, exception);

        /**
         * Constructs a null ServerSocket object.
         */
        ServerSocket() throw(exception);

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw();

    private:
        RefHandle<ServerSocketImpl> impl;
    };
}

#endif /* _NBPP_SERVER_SOCKET */
