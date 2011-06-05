//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SOCKET
#define _NBPP_SOCKET

#include <nb++/SocketImpl.hpp>

namespace nbpp
{
    typedef RefHandle<SocketImpl> SocketHandle;

    /**
	   Represents a communication socket. It make it possible to retrive both a
	   input or an output stream, to the socket.

	   The input stream is somehow special, as it try to read as much as possible
	   from the socket, before going in blocked mode. It is also possible to indicate
	   the max amount of bytes to be read form the socket if the protocol dictates
	   things like this (like in http). All this hopfully eliminate the old (new ?)
	   non block mode, as this normaly gives a good natural C++ program flow ... I
	   hope.

	   This class is reference-counted.
     */
    class Socket
    {
    public:
        /**
		   Connects to the specified address, with an optional timeout in seconds.

		   @param address the address of the socket to open
		   @param timeoutSec timeout walue in seconds

		   @exception TimeoutException if a non-zero timeout is given and
		   the operation times out.

		   @exception ConnectException if the connection attempt fails.
		*/
        explicit Socket(NetworkAddress address, int timeoutSec = 0)
            throw(IOException, AssertException, exception);

        /**
		   Wraps an existing connection to the specified address and port.

		   @param fd the connection handle to a already open socket
		   @param networkAddress the address of the given socket
         */
        Socket(int fd, NetworkAddress networkAddress) throw(AssertException, exception);

        /**
		   Returns an input stream to the current socket. Please remark that this
		   stream is blocking, but befor blocking it will try to read ALL available
		   data from the socket, before it blockes.

		   If the setMaxBytes have been set and the amount of bytes have been read
		   the stream will be set in eof mode.

		   If the socket has setup an timeout value it will throw an TimeoutException
		   if no data is available within this periode.

		   The stream will exist for as long as this socket class, after this it
		   will be invalid.

		   @see setMaxBytes

		   @note This it no longer deprecated, as planed, as the behaveur have changed

		   @return an input stream for this socket.
         */
        istream& getInputStream() const throw(AssertException, exception);

        /**
		   Returns an output stream to the current socket.

		   The stream will exist for as long as this socket class, after this it
		   will be invalid.

		   @return an output stream for this socket.
         */
        ostream& getOutputStream() const throw(AssertException, exception);

        /**
          Checks if there is input on this socket's input stream, and returns
		  immediately.

          @return true if there is input on this socket's input stream.
		  @exception SocketException if the connection is broken.
         */
        bool checkForInput() throw(IOException, AssertException, exception);

		/**
		   Setup the max amount of bytes to receive from this socket. Usefull for
		   protocols like HTTP, to make sure not to end up in block mode.

           -1 reset the socket to ignore the maxbytes

		   @param nMaxBytes The maximum number of bytes to read from now on.
		 */
		void setMaxBytes( int nMaxBytes );

        /**
		   @return the file descriptor of the socket
         */
        const int getFd() const throw(AssertException, exception);

        /**
		   @return the local address to which this socket is connected.
         */
        NetworkAddress getLocalAddress(NetworkAddress addr) const
            throw(IOException, AssertException, exception);

        /**
		   @return the local address to which this socket is connected.
         */
        NetworkAddress getLocalAddress() const
            throw(IOException, AssertException, exception);

        /**
		   @return the remote peer address to which this socket is connected.
         */
        NetworkAddress getPeerAddress(NetworkAddress addr) const
            throw(IOException, AssertException, exception);

        /**
		   @return the remote peer address to which this socket is connected.
         */
        NetworkAddress getPeerAddress() const
            throw(IOException, AssertException, exception);

        /**
		   Closes the socket.  This is done automatically by the destructor.
         */
        void close() throw(AssertException, exception);

        /**
		   Enables/disables a timeout, in seconds, for the waitForInput()
		   and waitForOutput() methods.  A value of 0 means an infinitely
		   long timeout.

		   @param timeoutSec the amount of seconds to wait, or 0
         */
        void setTimeout( int timeoutSec ) throw(AssertException, exception);

        /**
		   @return the timeout, in seconds, that has been set for this
		   socket.  A value of 0 means an infinitely long timeout.
         */
        int getTimeout() const throw(AssertException, exception);

        /**
		   Waits until there is input on this socket's input stream.  If a timeout has
		   been set, waits at most that much time.

		   The input stream is able to de the same, and will also throw an TimeoutException
		   if no data have been found on the socket connection for the given time.

		   @exception TimeoutException and closes the connection if the
		   operation times out.

		   @exception SocketException if the connection is broken.
         */
        void waitForInput() throw(IOException, AssertException, exception);

        /**
		   Waits until it's okay to send output on this socket's output stream.  If a
		   timeout has been set, waits at most that much time.  In order for this method
		   to work, your program must ignore SIGPIPE.

		   @exception TimeoutException and closes the connection if the
		   operation times out.

		   @exception SocketException if the connection is broken.
         */
        void waitToSend() throw(IOException, AssertException, exception);

        /**
		   Constructs a null Socket object.
         */
        Socket() throw(exception);

        /**
		   @return true if this object is non-null.
         */
        operator bool() const throw();

    private:
        SocketHandle impl;
    };
}

#endif /* _NBPP_SOCKET */
