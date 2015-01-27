/**
   Impliment the HTTPServer.
*/

#include <nb++/HTTPServer.hpp>
#include <nb++/Multipart.hpp>

#include <sstream>
#include <iostream>
#include <cstdio>

#include <stdexcept>
#include <algorithm>
#include <sys/stat.h>

using namespace nbpp;
using namespace std;

namespace std {
	inline bool operator==(const pair<string, string> &p, const string &str )
	{
		return p.first == str;
	}
}

////////////////////////////
// Impl. of HTTPRequest

static double split_request( const string &sLine, string &sMethod, string &sUrl )
{
	float nVersion = 0.9;
	istringstream is( sLine );

	is >> sMethod;
	if( is.get() == ' ' ) {
		is >> sUrl;
		if( is.get() == ' ' ) {
			string sVersion;
			is >> sVersion;
			sscanf( sVersion.c_str(), "HTTP/%f", &nVersion );
		}
	}
	return nVersion;
}

static bool split_head( const string &sLine, string &sName, string &sValue )
{
	string::size_type pos = sLine.find( ':' );

	if( pos == string::npos )
		return false;

	sName = sLine.substr( 0, pos );
	sValue = sLine.substr( pos+2, string::npos );

	return true;
}

static string make_etag( struct stat &stat )
{
    stringstream etag;

    etag << hex << "\"" << stat.st_ino << "-" << stat.st_mtime << "-" << stat.st_size << "\"";

    // return md5( etag.str() );
    return etag.str();
}

Request::Request( Socket &sock ) : _sock( sock ), _method( UNKNOWN ), _takeover( false ), _header_send( false ) {}

Request::~Request()
{
    if( !_header_send )
		send_out_header();

	ostream &os = _sock.getOutputStream();
	if( !_os.str().empty())
		os << _os.str();

	os.flush();
    _sock.waitToSend();
}

ostream &Request::getOutputStream()
{
    return _os;
}

bool Request::has_a( const string &name ) const
{
    string lname = string_tolower( name );

    replace( lname.begin(), lname.end(), '-', '_' );

    return _header_in.count( lname ) > 0;
}

string Request::get( const string &name ) const
{
    string lname = string_tolower( name );

    replace( lname.begin(), lname.end(), '-', '_' );

    values_t::const_iterator i = _header_in.find( lname );

    if( i != _header_in.end())
        return i->second;

    throw invalid_argument( "unknown header value '" + name + "' in request" );
}

void Request::append( const string &name, const string &data )
{
    _header_out[ name ] = data;
}

void Request::send_out_header( HTTPRequestHandler::Result res )
{
    _header_send = true;
}

Request::langs_t Request::accept_language_list() const
{
    langs_t langs;

    if( has_a( "Accept-Language" )) {
        string accept = get( "Accept-Language" );

        Strings parts = split( accept, "," );

        for( Strings::iterator i = parts.begin(); i != parts.end(); i++ ) {
            float quality = 1;
            Strings elms = split( *i, ";" );

            // Find quality parameter
            if( elms.size() > 1 ) {
                Strings::iterator pi = elms.begin();
                pi++;
                for( ; pi != elms.end(); pi++ ) {
                    Strings p = split( *pi, "=" );

                    if( p.size() == 2 ) {
                        if( p[0] == "q" )
                            quality = atof( p[1].c_str() );
                    }
                }
            }

            if( elms.size() > 0 )
                langs.push_back( make_pair(quality, trim(elms[ 0 ])));
        }
    }

    return langs;
}

float Request::accept_language( const string &locale ) const
{
    langs_t langs = accept_language_list();

    for( langs_t::iterator i = langs.begin(); i != langs.end(); i++ )
        if( i->second == locale )
            return i->first;

    return 0;
}

float Request::accept_mimetype( const string &mime_type ) const
{
    bool ok = false;
    float quality = 1;

    Strings rtypes = split( mime_type, "/" );

    if( has_a( "Accept" ) && rtypes.size() == 2) {
        string rtype = trim( rtypes[ 0 ] );
        string rsubtype = trim( rtypes[ 1 ] );
        string accept = get( "Accept" );

        Strings parts = split( accept, "," );

        for( Strings::iterator i = parts.begin(); i != parts.end(); i++ ) {
            Strings elms = split( *i, ";" );

            // Compare types and suptypes
            if( elms.size() > 0 ) {
                Strings types = split( elms[ 0 ], "/" );

                string type = trim(types[ 0 ]), subtype = trim( types[ 1 ] );

                if( type == rtype ) {
                    if( subtype == rsubtype ) {     // type/subtype
                        ok = true;
                    } else if( subtype == "*" )     // type/*
                        ok = true;
                } else if( type == "*" )            // */*
                    ok = true;
            }

            // Find quality parameter
            if( elms.size() > 1 ) {
                Strings::iterator pi = elms.begin();
                pi++;
                while( pi != elms.end()) {
                    if( !(*pi).empty()) {
                        Strings p = split( *pi, "=" );
                        if( p.size() == 2 ) {
                            if( p[0] == "q" )
                                quality = atof( p[1].c_str() );
                        }
                    }
                    pi++;
                }
            }
        }

        // XXX, remove when we are sure this works as expected
        /*clog << "INFO: request mime '" << mime_type << "' in '" << req[ "Accept" ] << "'";
        if( ok )
            clog << " has quality " << quality << " and is accepted " << endl;
        else
            clog << " is rejected" << endl;*/
    }

    return ok ? quality : 0;
}

bool Request::sendFile( const string &sFname )
{
    struct stat stat_buf;
    if( 0 == stat( sFname.c_str(), &stat_buf )) {
        char szSize[ 40 ];

        // Calculate etag, set it and test if it match user provided
        string etag = make_etag( stat_buf );
        append( "ETag", etag );
        if( has_a( "If-None-Match" )) {
            if( etag == get( "If-None-Match" ) || "*" == get( "If-None-Match" ) ) {
                send_out_header( HTTPRequestHandler::HTTP_NOT_MODIFIED );
                return true;
            }

            /*sendHTTPHeaders( HTTPRequestHandler::HTTP_PRECONDITION_FAILED );
            return false;*/
        }

        if( _method == HEAD ) {
            append( "Content-Length", stat_buf.st_size );

            send_out_header();
        } else {
            append( "Content-Length", stat_buf.st_size );

            // Copy the file directly to the stream
            ifstream is( sFname.c_str());
            ostream &os = _sock.getOutputStream();

            send_out_header();

            while( !is.eof() && os.good() && stat_buf.st_size ) {
                    char szBuffer[ 1024 ];
                    int nChunk = sizeof( szBuffer );

                    if( stat_buf.st_size < (off_t)sizeof( szBuffer ))
                        nChunk = stat_buf.st_size;

                    is.read( szBuffer, nChunk );
                    os.write( szBuffer, nChunk );
                    stat_buf.st_size -= nChunk;
            }
        }
            return true;
    }
    return false;
}

bool Request::sendFile( const string &sFname, size_t offset, size_t length )
{
	struct stat stat_buf;

	if( 0 == stat( sFname.c_str(), &stat_buf )) {
		char szSize[ 40 ];

        // Calculate etag, set it and test if it match user provided
        string etag = make_etag( stat_buf );
        append( "ETag", etag );
        if( has_a( "If-None-Match" )) {
            if( etag == (*this)[ "If-None-Match" ] || "*" == (*this)[ "If-None-Match" ] ) {
                send_out_header( HTTPRequestHandler::HTTP_NOT_MODIFIED );
                return true;
            }

            /*sendHTTPHeaders( HTTPRequestHandler::HTTP_PRECONDITION_FAILED );
            return false;*/
        }

        if( _method == HEAD ) {
            append( "Content-Length", stat_buf.st_size );

            send_out_header( HTTPRequestHandler::HTTP_OK );
        } else {
            if( length == 0 || length > stat_buf.st_size - offset )
               length = stat_buf.st_size - offset;

            append( "Content-Length", length );

            if( length == stat_buf.st_size ) {
                send_out_header(HTTPRequestHandler::HTTP_OK);
            } else {
                stringstream ros;

                ros << "bytes " << offset << "-" << (offset + length - 1) << "/" << stat_buf.st_size;
                append( "Content-Range", ros.str());

                send_out_header( HTTPRequestHandler::HTTP_PARTIAL_CONTENT );
            }

            // Copy the file directly to the stream
            ifstream is( sFname.c_str());
            ostream &os = _sock.getOutputStream();

            if( offset != 0 )
                is.seekg( offset, ios::beg );

            while( !is.eof() && os.good() && length ) {
                char szBuffer[ 1024 ];
                int nChunk = sizeof( szBuffer );

                if( length < (size_t)sizeof( szBuffer ))
                    nChunk = length;

                is.read( szBuffer, nChunk );
                if( is )
                    os.write( szBuffer, nChunk );

                length -= nChunk;
            }
        }
		return true;
	}
	return false;
}


ostream &Request::dump( ostream &os ) const
{
    os << "Request method ";
    switch( _method ) {
        case PUT:
            os << "PUT";
            break;
        case GET:
            os << "GET";
            break;
        case POST:
            os << "POST";
            break;
        case HEAD:
            os << "HEAD";
            break;
        case DELETE:
            os << "DELETE";
            break;
    };

    os << endl << "URI: " << _url.toString() << endl;

    os << "header in : " << endl;
    values_t::const_iterator i;
    for( i = _header_in.begin(); i != _header_in.end(); i++ )
        os << " " << i->first << ": " << i->second << endl;

    if( _header_out.size() > 0) {
        os << "header out : " << endl;
        for( i = _header_out.begin(); i != _header_out.end(); i++ )
            os << " " << i->first << ": " << i->second << endl;
    }
    return os;
}

/////////////////////////
// Impl. of HTTPRequest

HTTPRequest::HTTPRequest( Socket &socket ) : Request( socket )
{
    istream &is = _sock.getInputStream();

    // Read the request line
    string sMethod, sUrl, sLine;
    getline( is, sLine );
    // clog << "Request line : " << sLine << endl;
    _nVersion = split_request( sLine, sMethod, sUrl );

    if( sMethod == "GET" )
        _method = GET;
    else if( sMethod == "PUT" )
        _method = PUT;
    else if( sMethod == "POST" )
        _method = POST;
    else if( sMethod == "HEAD" )
        _method = HEAD;
    else if( sMethod == "DELETE" )
        _method = DELETE;
    else
        clog << "Unknown HTTP method " << sMethod << endl;

	_url.set( sUrl );
	// cout << "URI " << m_url.getUri() << endl;

	// m_socket.waitForInput();

	// Get the header elements if needed
	if( _nVersion > 0.9 )
		do {
			sLine.erase();

			if( !is.eof()) {
				string name, value;

				getline( is, sLine );
				if( *sLine.rbegin() == '\r' )
					sLine.resize( sLine.length() - 1 );

				if( split_head( sLine, name, value )) {
                    string n = string_tolower( name );

                    if( n.find( '-' ))
                        replace( n.begin(), n.end(), '-', '_' );

                    if( _header_in.count( n ) == 0 )
                        _header_in[ n ] = value;
                    else
                        _header_in[ n ] += "; " + value;
                }
			}
		} while( !sLine.empty());

	// get ready for content reading ... well we are !
	append( "Location", _url.toString() );
	// set( "Content-Type", "text/plain" );
	// cout << "Request accepted" << endl;

	if( has_a( "Content-Length" ))
		_sock.setMaxBytes( atoll((*this)[ "Content-Length" ].c_str()));
    else
        _sock.setMaxBytes( 0 );
}

void HTTPRequest::send_out_header( HTTPRequestHandler::Result res )
{
	if( _nVersion > 0.9 && !_header_send ) {
        // Read all that is left in case of error
        if( has_a( "Content-Length" )) {
            istream &is = getInputStream();

            while( !is.eof())
                is.get();
        }

		ostream &os = _sock.getOutputStream();

        if( _method != HEAD ) {
            values_t::const_iterator hi = _header_out.find( "Content-Length" );

            if( !_os.str().empty() && hi == _header_out.end())
                append( "Content-Length", _os.str().length() );
        }

		// Make status line
        if( os.good() ) {
            os << "HTTP/" << (_nVersion < 1.1 ? "1.0 " : "1.1 ") << res << " " << getHttpError( res ) << "\r\n";

            // send header fields
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
}

ostream &HTTPRequest::dump( ostream &os ) const
{
    os << "HTTP version " << _nVersion << endl;
    return Request::dump( os );
}

/////////////////////////////
// Impl. of HTTPFilter

// Only path is checks as for now
bool HTTPFilter::match( const URL &url ) const
{
	Strings::const_iterator iter;
	for( iter = m_paths.begin(); iter != m_paths.end(); iter++ ) {
		string sPath( url.getPath(), 0, iter->length());

		if( *iter == sPath )
			return true;
	}
	return false;
}

///////////////////////////
// Impl. of HTTPServer


struct HttpError {
	HTTPRequestHandler::Result res;
	const char *pszError;
	const char *pszDesc;
} http_errors[] = {
    {HTTPRequestHandler::HTTP_CONTINUE,                 "Continue","Continue"},
    {HTTPRequestHandler::HTTP_SWITCHING_PROTOCOLS,      "Switching Protocols","Switching Protocols"},
	{HTTPRequestHandler::HTTP_OK,                       "OK", "OK"},
	{HTTPRequestHandler::HTTP_CREATED,                  "Created", "Created"},
	{HTTPRequestHandler::HTTP_ACCEPTED,                 "Accepted", "Accepted"},
	{HTTPRequestHandler::HTTP_NO_CONTENT,               "No Content", "No Content"},
	{HTTPRequestHandler::HTTP_MOVED_PERMANENTLY,        "Moved Permanently", "Moved Permanently"},
	{HTTPRequestHandler::HTTP_MOVED_TEMPORARILY,        "Moved Temporarily", "Moved Temporarily"},
	{HTTPRequestHandler::HTTP_NOT_MODIFIED,             "Not Modified", "Not Modified"},
	{HTTPRequestHandler::HTTP_BAD_REQUEST,              "Bad Request", "Bad Request"},
	{HTTPRequestHandler::HTTP_UNAUTHORIZED,             "Unauthorized", "Unauthorized"},
	{HTTPRequestHandler::HTTP_FORBIDDEN,                "Forbidden",    "Forbidden"},
	{HTTPRequestHandler::HTTP_NOT_FOUND,                "Not Found",    "Service or document is not found"},
	{HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR,    "Internal Server Error", "Internal Server Error"},
	{HTTPRequestHandler::HTTP_NOT_IMPLEMENTED,          "Not Implemented", "Not Implemented"},
	{HTTPRequestHandler::HTTP_BAD_GATEWAY,              "Bad Gateway", "Bad Gateway"},
	{HTTPRequestHandler::HTTP_SERVICE_UNAVAILABLE,      "Service Unavailable", "Service Unavailable"},
    {HTTPRequestHandler::HTTP_RESET_CONTENT,            "Reset Content","Reset Content"},
    {HTTPRequestHandler::HTTP_PARTIAL_CONTENT,          "Partial Content","Partial Content"},
    {HTTPRequestHandler::HTTP_USE_PROXY,                "Use Proxy","Use Proxy"},
    {HTTPRequestHandler::HTTP_TEMPORARY_REDIRECT,       "Temporary Redirect","Temporary Redirect"},
    {HTTPRequestHandler::HTTP_METHOD_NOT_ALLOWED,       "Method Not Allowed","Method Not Allowed"},
    {HTTPRequestHandler::HTTP_NOT_ACCEPTABLE,           "Not Acceptable","Not Acceptable"},
    {HTTPRequestHandler::HTTP_PROXY_AUTHENTICATION_REQUIRED,"Proxy Authentication Required","Proxy Authentication Required"},
    {HTTPRequestHandler::HTTP_REQUEST_TIMEOUT,          "Request Timeout","Request Timeout"},
    {HTTPRequestHandler::HTTP_CONFLICT,                 "Conflict","Conflict"},
    {HTTPRequestHandler::HTTP_GONE,                     "Gone","Gone"},
    {HTTPRequestHandler::HTTP_LENGTH_REQUIRED,          "Length Required","Length Required"},
    {HTTPRequestHandler::HTTP_PRECONDITION_FAILED,      "Precondition Failed","Precondition Failed"},
    {HTTPRequestHandler::HTTP_REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large","Request Entity Too Large"},
    {HTTPRequestHandler::HTTP_REQUEST_URI_TOO_LONG,     "Request Uri Too Long","Request Uri Too Long"},
    {HTTPRequestHandler::HTTP_UNSUPPORTED_MEDIA_TYPE,   "Unsupported Media Type","Unsupported Media Type"},
    {HTTPRequestHandler::HTTP_REQUESTED_RANGE_NOT_SATISFIABLE,"Requested Range Not Satisfiable","Requested Range Not Satisfiable"},
    {HTTPRequestHandler::HTTP_EXPECTATION_FAILED,       "Expectation Failed","Expectation Failed"},
    {HTTPRequestHandler::HTTP_GATEWAY_TIMEOUT,          "Gateway Timeout","Gateway Timeout"},
    {HTTPRequestHandler::HTTP_VERSION_NOT_SUPPORTED,    "Version Not Supported","Version Not Supported"}
};

#define HTTP_ERROR_SIZE (sizeof( http_errors ) / sizeof( HttpError ))

const char *nbpp::getHttpError( HTTPRequestHandler::Result res, const char **desc )
{
	for( int i = 0; i != HTTP_ERROR_SIZE; i++ ) {
		if( http_errors[ i ].res == res ) {
            if( desc != NULL )
                *desc = http_errors[ i ].pszDesc;

			return http_errors[ i ].pszError;
		}
	}
    return NULL;
}

void sendHttpError( HTTPRequest &req, HTTPRequestHandler::Result res )
{
    if( !req.header_send()) {
        const char *pszErrorDesc = NULL;
        const char *pszError = getHttpError( res, &pszErrorDesc );

        req.append( "Content-Type", "text/html" );
        req.append( "Cache-Control", "max-age=0, no-cache" );

        ostream &os = req.getOutputStream();

        os << "<HTML><HEAD>";
        os << "<TITLE>" << pszError << "</TITLE>";
        os << "<HEAD><BODY>";
        os << "<H1>" << pszError << "</H1>";
        os << "<P>" << pszErrorDesc << "</P>";
        os << "</BODY></HTML>";
    }
}

HTTPServer::HTTPServer( const string &sPrgName ) : NetworkDaemon<InetAddress>( sPrgName ),
												   m_inetAddr( InetAddress::getAnyLocalHost( 8080 ))
{
}

void HTTPServer::setInetAddress( InetAddress iadr )
{
   m_inetAddr = iadr;
}

ServerSocket HTTPServer::initServerSocket( void ) throw( std::exception, Exception )
{
	return ServerSocket( m_inetAddr );
}

void HTTPServer::handleConnection( NetworkConnection<InetAddress> &connection )
	throw( exception, Exception )
{
	try {
        bool persist = true;
		connection.sock.setTimeout( 10 ); // Prevent socket to initially hang

        while( persist ) {
            connection.sock.setMaxBytes( -1 );

            HTTPRequestHandler::Result res = HTTPRequestHandler::HTTP_OK;

            // Check if more bytes and stop connection if socket is closed
            istream &is = connection.sock.getInputStream();
            if( !is.eof() && -1 != is.get())
                is.unget();

            if( !is )
                break;

            HTTPRequest req( connection.sock );
            if( req.getVersion() < 1.1 )
                persist = false;

            if( req.has_a( "Connection" ) && req[ "Connection" ] == "close" )
                persist = false;

            handlers_t::iterator i = m_handlers.begin();

            // cout << "Reg request .. " << endl;

            if( req.getMethod() == Request::POST && !req.has_a( "Content-Length" )) {
                res = HTTPRequestHandler::HTTP_BAD_REQUEST;
                persist = false;
            } else {
                try {
                    // dispatch logic
                    for( ; i != m_handlers.end(); i++ ) {
                        if( i->filter.match( req.getUrl() ) ) {
                            res = i->hndl->handle( req );

                            if( req.has_takeover() ) // Ok, the handler has taken over from here !
                                return;

                            break;
                        }
                    }

                    if( i == m_handlers.end())
                        res = HTTPRequestHandler::HTTP_NOT_FOUND;

                    if( res >= 400 && req.getOutStreamSize() == 0)
                        sendHttpError( req, res );

                    // IE need this in a none persistent connection, so we stop here !
                    if( res == 301 )
                        persist = false;
                } catch( const exception &ex ) {
                    clog << "ERROR: handling HTTP request " << ex.what() << endl;
                    res = HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR;
                    sendHttpError( req, res );
                }
            }

            if( !persist && req.getVersion() <= 1.1 )
                req.append( "Connection", "close" );

            req.send_out_header( res );
        }
	} catch( const Exception &ex ) {
		cerr << "ERROR: " << ex.toString() << endl;
	} catch( const exception &ex ) {
		cerr << "ERROR: " << ex.what() << endl;
	}
}

void HTTPServer::addHandler( HTTPFilter &filter, HTTPRequestHandler *pHndl )
{
	m_handlers.push_back( Handler( filter, pHndl ));
}

////////////////////////////
// Impl. of FieldStorage

/**
   Construct a FieldStorage from an input stream. Reads stream until end
   of stream. The input must be raw url encoded data, and the storage has the
   possibility to ignore blank values.

   @param is the stream to the raw URL encoded string.
   @param bKeepBlanks tell the class to keep any empty or blank value
 */
FieldStorage::FieldStorage( istream &is, bool keepBlanks )
{
    parse( is, keepBlanks );
}

FieldStorage::FieldStorage( Request &req, bool keepBlanks )
{

    if( req.has_a( "Content-Type" )) {
        string t( req[ "Content-Type" ] ); 

        if( t.substr( 0, 19 ) == "multipart/form-data" ) {
            Multipart mp( req );

            for( size_t i = 0; i < mp.size(); i++ ) { 
                string val = dynamic_cast<MemPart &>( mp[ i ] ).str();

                if( keepBlanks || !val.empty())
                    m_fields.push_back( make_pair( mp[ i ].name_get(), val ));
            }

            return;
        } else if( t == "application/x-www-form-urlencoded" ) {
           istream &is = req.getInputStream();

            parse( is, keepBlanks );

            return;
        } else
            throw invalid_argument( "unknown mime type for form" );
    }

    throw invalid_argument( "request is not a valid form type" );
}

/**
   Set a new string value to the FieldStorage

   @param sName is the name of the new value
   @param sData is the string data value
*/
void FieldStorage::append( const string &sName, const string &sData )
{
	m_fields.push_back( pair<string,string>( sName, sData ));
}

/**
   Set a new integer value to the FieldStorage

   @param sName is the name of the new value
   @param nData is the integer data value
*/
void FieldStorage::append( const string &sName, long nData )
{
	char szBuf[ 20 ];

	sprintf( szBuf, "%ld", nData );
	append( sName, szBuf );
}

Strings FieldStorage::fields() const {
    Strings res;

    for( vpdata_t::const_iterator i = m_fields.begin(); i != m_fields.end(); i++ )
        res.push_back( i->first );

    return res;
}

/**
   Check if this value name exist
*/
bool FieldStorage::hasA( const string &sName ) const
{
	vpdata_t::const_iterator iter;

	iter = find( m_fields.begin(), m_fields.end(), sName );

	return( iter != m_fields.end());
}

/**
   Fetch value in storage, and if not there an exeption is thrown !
*/
string FieldStorage::operator []( const string &sName ) const
{
	vpdata_t::const_iterator iter;

	iter = find( m_fields.begin(), m_fields.end(), sName );

	if( iter != m_fields.end())
		return iter->second;

	throw invalid_argument( "Can't find " + sName );
}

string FieldStorage::operator []( unsigned idx ) const
{
	return m_fields[ idx ].first;
}

string FieldStorage::conv_data( const string &str ) const
{
	string sResult;
	string::const_iterator i = str.begin();

	while( i != str.end()) {
		switch( *i ) {
    		case '&':
    		case '@':
    		case '/':
    		case '\'':
    		case '\"':
            case '%':
            case '\\':
    		case '=':
    		case '?':
            case '\n':
            case '\r':
            case '\t':
    		case '+': {
    			char szLine[ 10 ];

    			sprintf( szLine, "%%%02X", (unsigned)*i );
    			sResult += szLine;
    			break;
    		}

    		case ' ':
    			sResult += '+';
    			break;

    		default:
    			sResult += *i;
		}
		i++;
	}
	return sResult;
}

/**
    Convert the storage to a raw URL encoded string

    @return a new string containing the URL encoding
*/
string FieldStorage::toString( void ) const
{
	stringstream sstr;

	out( sstr );

	return sstr.str();
}

/**
   Convert the storage to a raw URL stream.

   @param os the output stream to put data into
*/
void FieldStorage::out( ostream &os ) const
{
	vpdata_t::const_iterator iter;

	for( iter = m_fields.begin(); iter != m_fields.end(); iter++ ) {
		if( iter != m_fields.begin())
			os << '&';

		os << iter->first << "=" << conv_data( iter->second );
	}
}

void FieldStorage::parse( istream &is, bool keepBlanks )
{
    string sName, sValue;
    bool bFetchValue = false;

    while(!is.eof()) {
        int ch;
        if((ch = is.get()) == -1 || ch == '\n' )
            break;

        if( ch == '&' ) { // add values
            if( keepBlanks || !sValue.empty())
                m_fields.push_back( make_pair( sName, sValue ));

            sName = sValue = "";
            bFetchValue = false;
            continue;
        }

        if( ch == '=' ) {
            bFetchValue = true;
            continue;
        }

        if( bFetchValue ) {
            switch( ch ) {
            case '%': { // Sorry about the mess, but gcc don't do it right
                char szBuf[ 3 ];

                szBuf[ 0 ] = (char)is.get();
                szBuf[ 1 ] = (char)is.get();
                szBuf[ 2 ] = 0;

                sValue += (char)strtol( szBuf, NULL, 16 );
                break;
            }
            case '+':
                sValue += ' ';
                break;

            default:
                sValue += ch;
            }
        }
        else
            sName += ch;
    }

    if( !sName.empty() )
        if( keepBlanks || !sValue.empty())
            m_fields.push_back( make_pair( sName, sValue ));
        

    if(m_fields.empty())
        throw invalid_argument( "parser error in FieldStoreage" );    
}

///////////////////////////////
// Impl. of HTTPFileHandler

HTTPFileHandler::HTTPFileHandler( const string &sUrlPath, const string &sFilePath )
{
	m_sUrlPath = sUrlPath;
	m_sFilePath = sFilePath;
	if( *m_sFilePath.rbegin() != '/' )
		m_sFilePath += '/';
}

string HTTPFileHandler::getMime( const string &sExt ) const
{
	exts_t::const_iterator i;

	for( i = m_exts.begin(); i != m_exts.end(); i++ )
		if( i->first == sExt )
			return i->second;

	return "";
}

void HTTPFileHandler::addMime( const string &sExt, const string &sMime )
{
	if( getMime( sExt ).empty())
		m_exts.push_back( make_pair( sExt, sMime ));
}

HTTPRequestHandler::Result HTTPFileHandler::handle( Request &req )
{
	URL url = req.getUrl();
	string sPath = url.getPath();

#if 0
	if( req.getMethod() == HTTPRequest::POST ) {
		char szData[ 1024 ];

		unsigned nLen = (unsigned)atoi( req[ "Content-Length" ].c_str() );
		nLen = nLen > sizeof( szData ) ? sizeof( szData ) : nLen;
		req.getInputStream().read( szData, nLen );
		szData[ nLen ] = 0;

		cout << "Data " << szData << endl;
	}
#endif

	if( sPath.find( m_sUrlPath ) == 0 ) {
		sPath = m_sFilePath + sPath.substr( m_sUrlPath.length());
		string sExt = sPath.substr( sPath.rfind( "." ) + 1 );

		// cout << "Looking for file : " << sPath << endl;

		if( !sExt.empty())
			req.append( "Content-Type", getMime( sExt ));

		if( req.sendFile( sPath ))
			return HTTP_OK;
	}

	return HTTP_NOT_FOUND;
}
