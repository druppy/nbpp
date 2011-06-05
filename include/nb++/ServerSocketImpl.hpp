//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SERVER_SOCKET_IMPL
#define _NBPP_SERVER_SOCKET_IMPL

#include <nb++/Socket.hpp>

namespace nbpp
{
    class ServerSocketImpl : public SocketBaseImpl
    {
    public:
        ServerSocketImpl(NetworkAddress networkAddress, int backlog)
            throw(IOException, AssertException, exception);

        Socket accept() const throw(IOException, AssertException, exception);
        Socket accept(NetworkAddress clientAddress) const
            throw(IOException, AssertException, exception);

    private:
        void init() throw(IOException, AssertException, exception);

        int backlog;
    };
}

#endif /* _NBPP_SERVER_SOCKET_IMPL */
