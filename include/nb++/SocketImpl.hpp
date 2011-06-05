//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_SOCKET_IMPL
#define _NBPP_SOCKET_IMPL

#include <nb++/osdep.hpp>

#include <iostream>
#include <fstream>

#include <nb++/Exception.hpp>
#include <nb++/SocketBaseImpl.hpp>

namespace nbpp
{
    using std::string;
    using std::istream;
    using std::ostream;
    using std::ifstream;
    using std::ofstream;
    using std::fstream;
    using std::iostream;

    class SocketImpl : public SocketBaseImpl
    {
    public:
        SocketImpl(NetworkAddress networkAddress, int timeout)
            throw(IOException, AssertException, exception);

        ~SocketImpl() throw();

        SocketImpl(int fd, NetworkAddress networkAddress) throw(AssertException, exception);

        ostream& getOutputStream() const throw(AssertException, exception);

        void waitForInput() throw(IOException, AssertException, exception);

		void setMaxBytes( int nMaxBytes ); // Total amount from current

        istream& getInputStream() const throw(AssertException, exception);

        bool checkForInput() throw(IOException, AssertException, exception);

        void waitToSend() throw(IOException, AssertException, exception);

    private:
        void connect() throw(IOException, AssertException, exception);

        void getStreams() throw(AssertException, exception);

        istream* in;
        ostream* out;

#if __GNUC__ >= 2
		std::streambuf *bin, *bout;  // Special brand !
#endif /* __GNUC__ >= 2 */
    };
}

#endif /* _NBPP_SOCKET_IMPL */
