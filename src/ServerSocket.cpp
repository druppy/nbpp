//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ServerSocket.hpp>

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
    ServerSocket::ServerSocket() throw(exception) { }

    ServerSocket::ServerSocket(NetworkAddress networkAddress, int backlog)
        throw(IOException, AssertException, exception) :
        impl(new ServerSocketImpl(networkAddress, backlog)) { }
    
    Socket ServerSocket::accept() const
        throw(IOException, AssertException, exception)
    {
        return impl->accept();
    }

    Socket ServerSocket::accept(NetworkAddress clientAddress) const
        throw(IOException, AssertException, exception)
    {
        return impl->accept(clientAddress);
    }

    const int ServerSocket::getFd() const throw(AssertException, exception)
    {
        return impl->getFd();
    }                                                                                    

    NetworkAddress ServerSocket::getLocalAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        return impl->getLocalAddress(addr);
    }

    NetworkAddress ServerSocket::getLocalAddress() const
        throw(IOException, AssertException, exception)
    {
        return impl->getLocalAddress();
    }

    NetworkAddress ServerSocket::getPeerAddress(NetworkAddress addr) const
        throw(IOException, AssertException, exception)
    {
        return impl->getPeerAddress(addr);
    }

    NetworkAddress ServerSocket::getPeerAddress() const
        throw(IOException, AssertException, exception)
    {
        return impl->getPeerAddress();
    }

    void ServerSocket::close() throw(AssertException, exception)
    {
        impl->close();
    }

    void ServerSocket::setTimeout(int sec) throw(AssertException, exception)
    {
        impl->setTimeout(sec);
    }

    int ServerSocket::getTimeout() const throw(AssertException, exception)
    {
        return impl->getTimeout();
    }

    ServerSocket::operator bool() const throw()
    {
        return impl;
    }
}
