
#ifndef _SCGISERVER_HPP
#define _SCGISERVER_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <nb++/HTTPServer.hpp>

namespace nbpp {

	using namespace std;

    class SCGIRequest : public Request {
    public:
        SCGIRequest( Socket &sock );
        
        void send_out_header(HTTPRequestHandler::Result res );
        
        ostream &dump( ostream &os ) const;
    };

    /**
        This is like the HTTP server but handles SCGI requestes instead
    */
    class SCGIServer : public HTTPServer {
	public:
        SCGIServer( const string &name ) : HTTPServer( name ) {
            setInetAddress( InetAddress::getAnyLocalHost( 9000 ));
        }
        
    protected:    
		// This may be called reentrant
		void handleConnection( NetworkConnection<InetAddress> &connection )
			throw( exception, Exception );
	};
}

#endif
