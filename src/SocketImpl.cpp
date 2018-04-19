//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/SocketImpl.hpp>

extern "C"
{
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
}

#if __GNUC__ == 2
#  ifdef _STLPORT_VERSION
#    include <fstream>
#  else
#    include <stdiostream.h>
#  endif
typedef int int_type;
#endif

#if __GNUC__ > 2
#include <ext/stdio_filebuf.h>
#endif

#include <streambuf>

namespace nbpp
{

	using namespace std;

	/**
	   Special class that first try to get all available data before blocking

	   More and less directly from josuttis book, thanks.
	 */
	class sock_inbuf : public std::streambuf {
		SocketImpl &m_socket;
		char m_buffer[ BUFSIZ ];
		int m_nCount, m_nMaxLength;
	public:
		sock_inbuf( SocketImpl &socket ) : m_socket( socket ) {
			setg( m_buffer + 4,
				  m_buffer + 4,
				  m_buffer + 4 );

			m_nMaxLength = -1;  // Not in use
			m_nCount = 0;
		}

		virtual ~sock_inbuf() {}

		void setMaxLength( int nLenght ) {
            if( nLenght != -1 )
                m_nMaxLength = nLenght + m_nCount - (egptr() - gptr());
            else
                m_nMaxLength = -1;
		}
	protected:

        virtual int_type underflow_simple() {
            if(gptr() < egptr())
                return *gptr();

            int num;
            if((num = recv(m_socket.getFd(), reinterpret_cast<char*>(m_buffer), BUFSIZ, 0)) <= 0)
                return traits_type::eof();

            setg(m_buffer, m_buffer, m_buffer + num);
            return *gptr();
        }

		virtual int_type underflow( void ) {
			// Is read buffer before the end ?
			if( gptr() > egptr())
				return traits_type::to_int_type( *gptr());

			// Putback ?
			int nPutback = gptr() - eback();
			if( nPutback > 4 )
				nPutback = 4;

			std::memcpy( m_buffer+(4-nPutback), gptr()-nPutback, nPutback ); // Is this optimal ?

			if( m_nMaxLength != -1 )
				if( m_nMaxLength <= m_nCount )
					return EOF;

			// First try using nonblock
			int nErrno, cnt;
			do {
				int nLen = BUFSIZ - 4;
				if( m_nMaxLength != -1 )
					nLen = (nLen < m_nMaxLength - m_nCount) ? nLen : m_nMaxLength - m_nCount;

                if( nLen == 0 )
                    return EOF;

                int nMask = fcntl( m_socket.getFd(), F_GETFL, 0);  // Get original mask

                if(fcntl(m_socket.getFd(), F_SETFL, nMask | O_NONBLOCK) < 0)
                    throw ConnectException( errno );

				errno = 0;
                
                // clog << "XXX: reading " << nLen << " from socket " << endl;

				cnt = read( m_socket.getFd(), m_buffer + 4, nLen );
				nErrno = errno; // Save errno

				if( cnt > 0 )
					m_nCount += cnt;

				if (fcntl( m_socket.getFd(), F_SETFL, nMask ) < 0)
					throw ConnectException( errno );

				// cout << "Stream read " << m_nMaxLength << ", " << cnt << ", " << m_nCount << endl;
				if( cnt <= 0 ) {
					if( nErrno == EAGAIN ) {
						// cout << "Begin wait " << m_nMaxLength << ", " << cnt << ", " << m_nCount << endl;
						m_socket.waitForInput();
						// cout << "End wait" << endl;
					}
					else
						return EOF;
				}
			} while( cnt <= 0 );

			setg( m_buffer + (4 - nPutback),
				  m_buffer + 4,
				  m_buffer + 4 + cnt);

			return traits_type::to_int_type( *gptr());
		}
	};

    SocketImpl::SocketImpl(NetworkAddress networkAddress, int timeoutSec)
        throw(IOException, AssertException, exception) :
        SocketBaseImpl(networkAddress, timeoutSec),
        in(0), out(0)
    {
        connect();
        getStreams();
    }

    SocketImpl::SocketImpl(int fd, NetworkAddress networkAddress)
        throw(AssertException, exception) :
        SocketBaseImpl(fd, networkAddress)
    {
        getStreams();
    }

    SocketImpl::~SocketImpl() throw()
    {
        if( in != NULL)
            delete in;

        if( out != NULL )
            delete out;
#if __GNUC__ >= 2
        delete bin;
        delete bout;
#endif
		close();
    }

    void SocketImpl::getStreams() throw(AssertException, exception)
    {
		// Some work is needed here to ensure other compilers works to regarding
		// initealizing the socket streams.

#if __GNUC__ >= 2
        in = new ifstream;
        out = new ofstream;
        in->tie( out );

		bin = new sock_inbuf( *this );
#  if __GNUC__ == 2
#    ifdef _STLPORT_VERSION
		bout = new filebuf();
		((filebuf *)bout)->open( m_fd );
#    else
		bout = new filebuf( m_fd );
#    endif
		in->rdbuf( bin );
		out->rdbuf( bout );
#  else
		// Make a stream dup to prevend filebuf from leaking its FILE struct
		// after destruction, when not owning the stream
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
 		bout = new __gnu_cxx::stdio_filebuf<char>
            (dup(m_fd), std::ios_base::out );
#else
		bout = new __gnu_cxx::stdio_filebuf<char>
            (dup(m_fd), std::ios_base::out, true, BUFSIZ );
#endif

		in->std::basic_ios<char>::rdbuf(bin);
 		out->std::basic_ios<char>::rdbuf(bout);
#  endif

#else  /* of __GNUC__ > 2 */
        in = new ifstream( m_fd );
        out = new ofstream( m_fd );
        in->tie( out );
#endif
    }

    istream& SocketImpl::getInputStream() const throw(AssertException, exception)
    {
        return *in;
    }

	void SocketImpl::setMaxBytes( int nMaxBytes )
	{
		sock_inbuf *p = static_cast<sock_inbuf *>( bin );
		assert( p != NULL );
		p->setMaxLength( nMaxBytes );
	}

    void SocketImpl::connect() throw(IOException, AssertException, exception)
    {
        // Set nonblocking mode so we can time out the connection
        // attempt if necessary.
        if (fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL, 0) | O_NONBLOCK) < 0)
            throw ConnectException( errno );

        // Try to connect.
        ::connect(m_fd, address.getNativeForm (), address.getNativeFormSize ());

        // Wait until the connect attempt completes, or the timeout
        // expires.
        try {
            if (!wait(m_fd, false, false)) {
                int err = errno;
                close();
                throw TimeoutException( err, "Timed out waiting to connect" );
            }
        } catch ( const SocketException& e ) {
            throw ConnectException( e.getErrno() );
        }

        // The connection succeeded, so clear nonblocking mode.
        if (fcntl(m_fd, F_SETFL,
                  fcntl(m_fd, F_GETFL, 0) & ~O_NONBLOCK) < 0) {
            int nErrno = errno;
            close();
            throw ConnectException( nErrno );
        }
    }

    void SocketImpl::waitForInput() throw(IOException, AssertException, exception)
    {
		if (!(*in)) {
            int err = errno;
            close();
            throw SocketException( err, "Connection closed" );
        }

        if (in->rdbuf()->in_avail())
            return;


        if (!wait(m_fd, true, false)) {
            int err = errno;
            close();
            throw TimeoutException( err, "Timed out waiting for input" );
        }
    }

    // Added by Daniel Barron April 2001 daniel@jadeb.com
    bool SocketImpl::checkForInput() throw(IOException, AssertException, exception)
    {
        if (!(*in)) {
            int err = errno;
            close();
            throw SocketException( err, "Connection closed" );
        }

        if (in->rdbuf()->in_avail())
            return true;

        if (wait(m_fd, true, true))
            return true;

        return false;
    }

    ostream& SocketImpl::getOutputStream() const throw(AssertException, exception)
    {
        return *out;
    }

    void SocketImpl::waitToSend() throw(IOException, AssertException, exception)
    {
        if (wait(m_fd, false, false))
            return;
        else {
            int err = errno;
            close();
            throw TimeoutException( err, "Timed out waiting to send output" );
        }
    }
}
