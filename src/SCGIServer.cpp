#include <nb++/SCGIServer.hpp>
#include <cstdlib>
#include <stdexcept>

using namespace std;
using namespace nbpp;

static void sendSCGIError( Request &req, HTTPRequestHandler::Result res )
{
    if( !req.header_send()) {
        const char *pszErrorDesc = NULL;
        const char *pszError = getHttpError( res, &pszErrorDesc );
    
        req.append( "Status", pszError );
        req.append( "Content-Type", "text/plain" );
        
        ostream &os = req.getOutputStream();
        os << pszErrorDesc;
    }
}

void SCGIServer::handleConnection( NetworkConnection<InetAddress> &connection ) throw( exception, Exception )
{
	try {
		connection.sock.setTimeout( 10 ); // Prevent socket to initially hang

        connection.sock.setMaxBytes( -1 );

        HTTPRequestHandler::Result res = HTTPRequestHandler::HTTP_OK;

        SCGIRequest req( connection.sock );
                
        if( req.getMethod() == Request::POST && !req.has_a( "CONTENT_LENGHT" )) {
            res = HTTPRequestHandler::HTTP_BAD_REQUEST;
        } else {
            try {
                // dispatch logic
                handlers_t::iterator i = m_handlers.begin();

                for( ; i != m_handlers.end(); i++ ) {
                    if( i->filter.match( req.getUrl())) {
                        res = i->hndl->handle( req );

                        if( req.has_takeover() ) // Ok, the handler has taken over from here !
                            return;

                        break;                
                    }
                }

                if( i == m_handlers.end())
                    res = HTTPRequestHandler::HTTP_NOT_FOUND;

                if( res >= 400 && req.getOutStreamSize() == 0)
                    sendSCGIError( req, res );
            } catch( const exception &ex ) {
                res = HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR;
                sendSCGIError( req, res );
            }
    
            req.send_out_header( res );
        }
	} catch( const Exception &ex ) {
		clog << ex.toString() << endl;
	} catch( const exception &ex ) {
		clog << ex.what() << endl;
	}
}

static string netstring( istream &is )
{
    string length, str;
    int len = 0;
   
    while( is ) {
        char ch = is.get();
        
        if( len != 0 && str.length() < len )
            str += ch;
        
        if( isdigit( ch ))
            length += ch;
        else if( ch == ':' )
            len = atoi( length.c_str());
        else {
            if( str.length() == len ) {
                ch = is.get();
                
                if( ch == ',' )
                    break;
                else
                    throw invalid_argument( "bad netstring termination" );        
            }
        }
    }
    
    return str;
}
            
SCGIRequest::SCGIRequest( Socket &sock ) : Request( sock )
{
    // Parse the SCGI headers !!!
    string raw_head = netstring( sock.getInputStream());
    
    string::const_iterator s = raw_head.begin();
    string name;
    for( string::const_iterator i = raw_head.begin(); i != raw_head.end(); i++ ) {
        if( *i == '\0' ) {
            if( name.empty()) {
                name = string( s, i );
            } else {
                _header_in[ string_tolower( name ) ] = string( s, i );
                name.clear();
            }
            s = i; s++;
        }
    }
    
    // Normalize header data for non CGI aware tools
    if( has_a( "content_length" ))
        _header_in[ "content-length" ] = get( "content_length" );
    
    if( has_a( "http_cookie" ))
        _header_in[ "cookie" ] = get( "http_cookie" );
        
    if( has_a( "http_accept" ))
        _header_in[ "accept" ] = get( "http_accept" );

    if( has_a( "http_accept_encoding" ))
        _header_in[ "accept-encoding" ] = get( "http_accept_encoding" );
        
    if( has_a( "http_accept_language" ))
        _header_in[ "accept-language" ] = get( "http_accept_language" );
        
    // validate headers and update values
    if( has_a( "content_length" ) ) {
        _sock.setMaxBytes( atoll(get( "content_length" ).c_str()));
        
        if( has_a( "scgi" ) && get( "scgi" ) == "1" ) {
            if( has_a( "request_method" )) {
                string m = get( "request_method" );
                
                if( m == "GET" )
                    _method = GET;
                else if( m == "PUT" )
                    _method = PUT;
                else if( m == "POST" )
                    _method = POST;
                else if( m == "HEAD" )
                    _method = HEAD;
                else if( m == "DELETE" )
                    _method = DELETE;
                else
                    throw invalid_argument( "unknown SCGI request method " + m );
            }
            
            if( has_a( "request_uri" )) 
                _url.set( get( "request_uri" )); 
        } else
            throw invalid_argument( "missing SCGI value in request" );
    } else
        throw invalid_argument( "missing CONTENT_LENGTH in SCGI request" );
}

void SCGIRequest::send_out_header(HTTPRequestHandler::Result res )
{
    if( !header_send()) {
        ostream &os = _sock.getOutputStream();

        const char *pszErrorDesc = NULL;
        const char *pszError = getHttpError( res, &pszErrorDesc );
        
        append( "Status", pszError );
     
        values_t::const_iterator i;
        for( i = _header_out.begin(); i != _header_out.end(); i++ ) {
            if( os.good() )
                os << i->first << ": " << i->second << "\r\n";
        }

        // Make ready for user content
        os << "\r\n";
        os.flush();
    }
    Request::send_out_header();
}
