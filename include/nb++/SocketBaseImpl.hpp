//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SOCKET_BASE_IMPL
#define _NBPP_SOCKET_BASE_IMPL

#include <nb++/osdep.hpp>

extern "C"
{
#ifdef NBPP_TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef NBPP_HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
}

#include <nb++/NetworkAddress.hpp>
#include <nb++/RWLocker.hpp>
#include <nb++/RefCount.hpp>

namespace nbpp
{
    /**
     * A base class for BSD socket implementations.
     */
    class SocketBaseImpl : public RefCounted
    {
    public:
        virtual ~SocketBaseImpl() throw() = 0;

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
         * Enables/disables a timeout, in seconds.  Subclasses use
         * this timeout in different ways.  A value of 0 means an
         * infinitely long timeout.
         */
        void setTimeout(int sec) throw(AssertException, exception);

        /**
         * @return the timeout, in seconds, that has been set for this
         * socket.  A value of 0 means an infinitely long timeout.
         */
        int getTimeout() const throw(AssertException, exception);

        /**
         * @return the file descriptor of the socket
         */
        const int getFd() const throw(AssertException, exception); 

    protected:
        /**
         * Creates a SocketBaseImpl that isn't connected to anything.
         */
        SocketBaseImpl() throw(AssertException, exception);

        /**
         * Gets a socket from the kernel, and fills in address and
         * timeoutSec.
         */
        SocketBaseImpl(NetworkAddress address, int timeoutSec = 0)
            throw(IOException, AssertException, exception);

        /**
         * Wraps an already-connected socket.
         */
        SocketBaseImpl(int fd, NetworkAddress networkAddress)
            throw(AssertException, exception);

        /**
         * Waits until the specified file descriptor is ready for input (if forInput is
         * true) or output (if forInput is false).  If a timeout has been set, waits at
         * most that much time.  After waiting, checks the file descriptor's error status;
         * if an error has occurred, the method throws a SocketException, and closes the
         * socket.
         *
         * @param forInput if true, waits for input; otherwise, waits for output.
         * @param justTest if true, tests the status of the file descriptor and returns
         * immediately.
         * @return true if the file descriptor is ready for the selected operation.
         * @exception IOException if an I/O error occurs.
         */
        bool wait(int fd, bool forInput, bool justTest) const
            throw(IOException, AssertException, exception);

        mutable int m_fd;
        NetworkAddress address;

    private:
        SocketBaseImpl(const SocketBaseImpl&);
        SocketBaseImpl& operator=(const SocketBaseImpl&);

        int timeoutSec;
        mutable RWLocker timeoutLock;
    };
}

#endif /* _NBPP_SOCKET_BASE_IMPL */
