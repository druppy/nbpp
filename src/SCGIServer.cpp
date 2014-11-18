#include <nb++/SCGIServer.hpp>
#include <cstdlib>
#include <stdexcept>
#include <memory>

using namespace std;
using namespace nbpp;

static void sendSCGIError( Request &req, HTTPRequestHandler::Result res )
{
    if( !req.header_send()) {
        const char *pszErrorDesc = NULL;
        const char *pszError = getHttpError( res, &pszErrorDesc );

        req.append( "Content-Type", "text/plain" );
        req.append( "Cache-Control", "max-age=0, no-cache" );

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

        if( req.getMethod() == Request::POST && !req.has_a( "content_length" )) {
            res = HTTPRequestHandler::HTTP_BAD_REQUEST;

            sendSCGIError( req, res );

            req.send_out_header( res );
        } else {
            try {
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
                clog << "ERROR: handling SCGI request " << ex.what() << endl;
                res = HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR;
                sendSCGIError( req, res );
            }

            req.send_out_header( res );
        }
	} catch( const Exception &ex ) {
		clog << "ERROR: " << ex.toString() << endl;
	} catch( const exception &ex ) {
		clog << "ERROR: " << ex.what() << endl;
	}
}

static string netstring( istream &is )
{
    string length, str;
    int len = 0;

    while( !is.eof() ) {
        char ch = is.get();

        if( len != 0 && str.length() < len )
            str += ch;
        else if( len != 0 && str.length() == len ) {
            if( ch == ',' )
                break;
            else
                throw invalid_argument( "bad netstring termination" );
        } else if( isdigit( ch ))
            length += ch;
        else if( ch == ':' )
            len = atoi( length.c_str());
    }

    return str;
}

SCGIRequest::SCGIRequest( Socket &sock ) : Request( sock )
{
    // Parse the SCGI headers !!!
    _max_bytes = -1;
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

    if( has_a( "content_type" ))
        _header_in[ "content-type" ] = get( "content_type" );

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

            _max_bytes = atoll(get( "content_length" ).c_str());
            _sock.setMaxBytes( _max_bytes );

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
        // Read all that is left in case of error
        if( has_a( "content_length" )) {
            istream &is = getInputStream();

            while( !is.eof())
                is.get();
        }

        ostream &os = _sock.getOutputStream();

        const char *pszErrorDesc = NULL;
        const char *pszError = getHttpError( res, &pszErrorDesc );

        // Must come first !
        os << "Status: " << res << " " << pszError << "\r\n";

        // Make sure the Content length are presend and valid
        if( _method != HEAD ) {
            values_t::const_iterator hi = _header_out.find( "Content-Length" );

            if( !_os.str().empty() && hi == _header_out.end())
                append( "Content-Length", _os.str().length() );
        }

        values_t::const_iterator i;
        for( i = _header_out.begin(); i != _header_out.end(); i++ ) {
            if( os.good() )
                os << i->first << ": " << i->second << "\r\n";
        }

        // Make ready for user content
        os << "\r\n";
        os.flush();
    
        Request::send_out_header();
    }
}

ostream &SCGIRequest::dump( ostream &os ) const
{
    os << "SCGI" << endl;
    return Request::dump( os );
}
