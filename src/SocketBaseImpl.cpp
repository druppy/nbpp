//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Log.hpp>
#include <nb++/SocketBaseImpl.hpp>
#include <nb++/Exception.hpp>

extern "C"
{
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
}

namespace nbpp
{
    SocketBaseImpl::SocketBaseImpl() throw(AssertException, exception) { }

    SocketBaseImpl::SocketBaseImpl(NetworkAddress arg_address, int arg_timeoutSec)
        throw(IOException, AssertException, exception) :
        address(arg_address), timeoutSec(arg_timeoutSec)
    {
        // Get a socket file descriptor from the kernel.
        if ((m_fd = socket(address.getNativeFormFamily(), SOCK_STREAM, 0)) < 0)
            throw IOException( errno );
    }

    SocketBaseImpl::SocketBaseImpl(int arg_fd, NetworkAddress arg_networkAddress)
        throw(AssertException, exception) :
        m_fd(arg_fd), address(arg_networkAddress), timeoutSec(0) { }

    SocketBaseImpl::~SocketBaseImpl() throw()
    {
        try {
            close();
        }
        catch (...) { }
    }

    void SocketBaseImpl::close() throw(AssertException, exception)
    {
        if (m_fd != -1) {
            ::close( m_fd );
            m_fd = -1;
        }
    }

    NetworkAddress SocketBaseImpl::getLocalAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        if (addr) {
            socklen_t len = addr.getNativeFormMaxSize ();
            unsigned char addrBuf [len];
            if (::getsockname(m_fd, reinterpret_cast<sockaddr*>(addrBuf), &len) < 0)
                throw SocketException( errno );
    
            addr.assignFromNativeForm(reinterpret_cast<sockaddr*>(addrBuf), len);
        }
        return addr;
    }

    NetworkAddress SocketBaseImpl::getLocalAddress() const
        throw(IOException, AssertException, exception)
    {
        return getLocalAddress(NetworkAddress(address.makeEmptyClone()));
    }

    NetworkAddress SocketBaseImpl::getPeerAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        if (addr) {
            socklen_t len = addr.getNativeFormMaxSize ();
            unsigned char addrBuf [len];
            if (::getpeername (m_fd, reinterpret_cast<sockaddr*>(addrBuf), &len) < 0)
                throw SocketException( errno );
    
            addr.assignFromNativeForm(reinterpret_cast<sockaddr*>(addrBuf), len);
        }
        return addr;
    }

    NetworkAddress SocketBaseImpl::getPeerAddress() const
        throw(IOException, AssertException, exception)
    {
        return getPeerAddress(NetworkAddress(address.makeEmptyClone()));
    }

    void SocketBaseImpl::setTimeout(int sec) throw(AssertException, exception)
    {
        WriteLock writeLock(timeoutLock);
        timeoutSec = sec;
    }

    int SocketBaseImpl::getTimeout() const throw(AssertException, exception)
    {
        ReadLock readLock(timeoutLock);
        return timeoutSec;
    }

    const int SocketBaseImpl::getFd() const throw(AssertException, exception)
    {
        return m_fd;
    }

    bool SocketBaseImpl::wait(int sock, bool forInput, bool justTest) const
        throw(IOException, AssertException, exception)
    {
		if( sock == -1 ) {
			nbpp::log.put( "Strange socket handler" );
			throw SocketException( "Strange socket handler" );
		}
			
        ReadLock readLock(timeoutLock);
        int localTimeoutSec = timeoutSec;
        readLock.release();

        timeval* timevalPtr;
        if (justTest) {
            timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
            timevalPtr = &timeout;
        }
        else {
            if (localTimeoutSec == 0)
                timevalPtr = NULL;
            else {
                timeval timeout;
                timeout.tv_sec = timeoutSec;
                timeout.tv_usec = 0;
                timevalPtr = &timeout;
            }
        }

        fd_set fdSet;
        FD_ZERO(&fdSet);
        FD_SET(sock, &fdSet);

        fd_set* inputSetPtr;
        fd_set* outputSetPtr;

        if (forInput) {
            inputSetPtr = &fdSet;
            outputSetPtr = NULL;
        }
        else {
            inputSetPtr = NULL;
            outputSetPtr = &fdSet;
        }

        if (select(static_cast<NBPP_SELECT_TYPE_ARG1>(sock + 1),
                   NBPP_SELECT_TYPE_ARG234 inputSetPtr,
                   NBPP_SELECT_TYPE_ARG234 outputSetPtr,
                   NBPP_SELECT_TYPE_ARG234 NULL,
                   NBPP_SELECT_TYPE_ARG5   timevalPtr) < 0)
        {
            throw SocketException( errno );
        }

        // Check the socket for errors.
        int soErrorVal;
        socklen_t soErrorValLen = sizeof(soErrorVal);
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &soErrorVal,
                       &soErrorValLen) < 0)
        {
            const_cast<SocketBaseImpl &>( *this ).close();
            throw SocketException( errno );
        }

        if (soErrorVal)
        {
            const_cast<SocketBaseImpl &>( *this ).close();
            throw SocketException( soErrorVal );
        }

        return FD_ISSET(sock, &fdSet);
    }
}
