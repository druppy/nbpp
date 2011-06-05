//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Socket.hpp>

extern "C"
{
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
}

namespace nbpp
{
    Socket::Socket() throw(exception) { }

    Socket::Socket(NetworkAddress address, int timeout)
        throw(IOException, AssertException, exception) :
        impl(new SocketImpl(address, timeout)) { }

    Socket::Socket(int fd, NetworkAddress networkAddress) throw(AssertException, exception) :
        impl(new SocketImpl(fd, networkAddress)) { }

    ostream& Socket::getOutputStream() const throw(AssertException, exception)
    {
        return impl->getOutputStream();
    }

    istream& Socket::getInputStream() const throw(AssertException, exception)
    {
        return impl->getInputStream();
    }

    bool Socket::checkForInput() throw(IOException, AssertException, exception)
    {
        return impl->checkForInput();
    }

	void Socket::setMaxBytes( int nMaxBytes ) 
	{
		impl->setMaxBytes( nMaxBytes );
	}

    const int Socket::getFd() const throw(AssertException, exception)
    {
        return impl->getFd();
    }                                                                                     

    void Socket::waitForInput() throw(IOException, AssertException, exception)
    {
        impl->waitForInput();
    }
    
    void Socket::waitToSend() throw(IOException, AssertException, exception)
    {
        impl->waitToSend();
    }


    NetworkAddress Socket::getLocalAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        return impl->getLocalAddress(addr);
    }

    NetworkAddress Socket::getLocalAddress() const
        throw(IOException, AssertException, exception)
    {
        return impl->getLocalAddress();
    }

    NetworkAddress Socket::getPeerAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        return impl->getPeerAddress(addr);
    }

    NetworkAddress Socket::getPeerAddress() const
        throw(IOException, AssertException, exception)
    {
        return impl->getPeerAddress();
    }

    void Socket::close() throw(AssertException, exception)
    {
        impl->close();
    }

    void Socket::setTimeout(int sec) throw(AssertException, exception)
    {
        impl->setTimeout(sec);
    }

    int Socket::getTimeout() const throw(AssertException, exception)
    {
        return impl->getTimeout();
    }

    Socket::operator bool() const throw()
    {
        return impl;
    }
}
