/**
   Impliment the HTTPServer.
*/

#include <nb++/HTTPServer.hpp>

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

static const char *getHttpError( HTTPRequestHandler::Result res, const char **desc = NULL );

/////////////////////////
// Impl. of HTTPRequest

HTTPRequest::HTTPRequest( Socket &socket ) : m_socket( socket )
{
    m_bHeaderSend = false;
    istream &is = m_socket.getInputStream();

    // Read the request line
    string sMethod, sUrl, sLine;
    getline( is, sLine );
    //  clog << "Request line : " << sLine << endl;
    m_nVersion = split_request( sLine, sMethod, sUrl );

    if( sMethod == "GET" )
	m_method = GET;
    else if( sMethod == "PUT" )
        m_method = PUT;
    else if( sMethod == "POST" )
        m_method = POST;
    else if( sMethod == "HEAD" )
        m_method = HEAD;
    else if( sMethod == "DELETE" )
        m_method = DELETE;
    else
        clog << "Unknown HTTP method " << sMethod << endl;

	m_url.set( sUrl );
	// cout << "URI " << m_url.getUri() << endl;

	// m_socket.waitForInput();

	// Get the header elements if needed
	if( m_nVersion > 0.9 )
		do {
			sLine.erase();

			if( !is.eof()) {
				string sName, sValue;

				getline( is, sLine );
				if( *sLine.rbegin() == '\r' )
					sLine.resize( sLine.length() - 1 );

				if( split_head( sLine, sName, sValue )) {
					sName = string_tolower( sName );
					m_header_in.push_back( NameValue( sName, sValue ));
					// clog << sName << ": " << sValue << endl;
					// m_socket.waitForInput();
				}
			}
		} while( !sLine.empty());

	// get ready for content reading ... well we are !
	set( "Location", m_url.toString() );
	set( "Content-Type", "text/plain" );
	// cout << "Request accepted" << endl;

	if( hasA( "Content-Length" ))
		m_socket.setMaxBytes( atoi((*this)[ "Content-Length" ].c_str()));
}

HTTPRequest::~HTTPRequest()
{
	if( !m_bHeaderSend )
		sendHTTPHeaders();

	ostream &os = m_socket.getOutputStream();
	if( !m_os.str().empty())
		os << m_os.str();

	os.flush();
    m_socket.waitToSend();
}

istream &HTTPRequest::getInputStream()
{
    return m_socket.getInputStream();
}

ostream &HTTPRequest::getOutputStream()
{
	return m_os;
}

Socket& HTTPRequest::getSocket( void )
{
	return m_socket;
}

HTTPRequest::Method HTTPRequest::getMethod( void ) const
{
	return m_method;
}

const URL &HTTPRequest::getUrl( void ) const
{
	return m_url;
}

bool HTTPRequest::hasA( const string &sName ) const
{
	values_t::const_iterator i = find( m_header_in.begin(), m_header_in.end(), string_tolower(sName ));

	return i == m_header_in.end() ? false : true;
}

string HTTPRequest::operator[]( const string &sName ) const
{
	values_t::const_iterator i = find( m_header_in.begin(), m_header_in.end(), string_tolower( sName ));

	if( i != m_header_in.end())
		return i->sValue;

	throw invalid_argument( sName + " is not a http header field in this request" );
}

void HTTPRequest::set( const string &sName, const string &sValue, bool sReplace )
{
    if (!sReplace) {
        m_header_out.push_back( NameValue( sName, sValue ));
        return;
    }

    // Erase all element with the given name
    values_t::iterator i = m_header_out.begin();
    while( i != m_header_out.end() ) {
        if( i->sName == sName )
            i = m_header_out.erase(i);
        else
            i++;
    }

    m_header_out.push_back( NameValue( sName, sValue ));
}

void HTTPRequest::set( const string &name, size_t val, bool replace )
{
    char buf[ 20 ];
    sprintf( buf, "%lu", val );
    set( name, buf, replace );
}

void HTTPRequest::sendHTTPHeaders( HTTPRequestHandler::Result res )
{
	if( m_nVersion > 0.9 && !m_bHeaderSend ) {
		ostream &os = m_socket.getOutputStream();

        values_t::const_iterator hi = find( m_header_out.begin(), m_header_out.end(), "Content-Length" );
        if( !m_os.str().empty() && hi == m_header_out.end())
			set( "Content-Length", m_os.str().length() );

		// Make status line
		os << "HTTP/" << (m_nVersion < 1.1 ? "1.0 " : "1.1 ") << res << " " << getHttpError( res ) << "\r\n";

		// send header fields
		values_t::const_iterator i;
		for( i = m_header_out.begin(); i != m_header_out.end(); i++ )
			os << i->sName << ": " << i->sValue << "\r\n";

		// Make ready for user content
		os << "\r\n";
		os.flush();
	}
	m_bHeaderSend = true;
}

bool HTTPRequest::sendFile( const string &sFname )
{
	struct stat stat_buf;
	if( 0 == stat( sFname.c_str(), &stat_buf )) {
		char szSize[ 40 ];

		set( "Content-Length", stat_buf.st_size );

		// Copy the file directly to the stream
		ifstream is( sFname.c_str());
		ostream &os = m_socket.getOutputStream();

		sendHTTPHeaders();

		while( !is.eof() && stat_buf.st_size ) {
			char szBuffer[ 1024 ];
			int nChunk = sizeof( szBuffer );

			if( stat_buf.st_size < (off_t)sizeof( szBuffer ))
				nChunk = stat_buf.st_size;

			is.read( szBuffer, nChunk );
			os.write( szBuffer, nChunk );
			stat_buf.st_size -= nChunk;
		}
		return true;
	}
	return false;
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
	{HTTPRequestHandler::HTTP_OK,                    "OK", "OK"},
	{HTTPRequestHandler::HTTP_CREATED,               "Created", "Created"},
	{HTTPRequestHandler::HTTP_ACCEPTED,              "Accepted", "Accepted"},
	{HTTPRequestHandler::HTTP_NO_CONTENT,            "No Content", "No Content"},
	{HTTPRequestHandler::HTTP_MOVED_PERMANENTLY,     "Moved Permanently", "Moved Permanently"},
	{HTTPRequestHandler::HTTP_MOVED_TEMPORARILY,     "Moved Temporarily", "Moved Temporarily"},
	{HTTPRequestHandler::HTTP_NOT_MODIFIED,          "Not Modified", "Not Modified"},
	{HTTPRequestHandler::HTTP_BAD_REQUEST,           "Bad Request", "Bad Request"},
	{HTTPRequestHandler::HTTP_UNAUTHORIZED,          "Unauthorized", "Unauthorized"},
	{HTTPRequestHandler::HTTP_FORBIDDEN,             "Forbidden",    "Forbidden"},
	{HTTPRequestHandler::HTTP_NOT_FOUND,             "Not Found",    "Service or document is not found"},
	{HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR, "Internal Server Error", "Internal Server Error"},
	{HTTPRequestHandler::HTTP_NOT_IMPLEMENTED,       "Not Implemented", "Not Implemented"},
	{HTTPRequestHandler::HTTP_BAD_GATEWAY,           "Bad Gateway", "Bad Gateway"},
	{HTTPRequestHandler::HTTP_SERVICE_UNAVAILABLE,   "Service Unavailable", "Service Unavailable"}
};
#define HTTP_ERROR_SIZE (sizeof( http_errors ) / sizeof( HttpError ))

const char *getHttpError( HTTPRequestHandler::Result res, const char **desc )
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
    const char *pszErrorDesc = NULL;
	const char *pszError = getHttpError( res, &pszErrorDesc );

	ostream &os = req.getOutputStream();

	os << "<HTML><HEAD>";
	os << "<TITLE>" << pszError << "</TITLE>";
	os << "<HEAD><BODY>";
	os << "<H1>" << pszError << "</H1>";
	os << "<P>" << pszErrorDesc << "</P>";
	os << "</BODY></HTML>";

	req.set( "Content-Type", "text/html" );
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

            if( req.hasA( "Connection" ) && req[ "Connection" ] == "close" )
                persist = false;

            handlers_t::iterator i = m_handlers.begin();

            // cout << "Reg request .. " << endl;

            if( req.getMethod() == HTTPRequest::POST && !req.hasA( "Content-Length" )) {
                res = HTTPRequestHandler::HTTP_BAD_REQUEST;
                persist = false;
            } else {
                try {
                    // dispatch logic
                    for( ; i != m_handlers.end(); i++ ) {
                        if( i->filter.match( req.getUrl() ) ) {
                            res = i->hndl->handle( req );
                            break;
                        }
                    }

                    if( i == m_handlers.end())
                        res = HTTPRequestHandler::HTTP_NOT_FOUND;

                    if( res > 300 )
                        sendHttpError( req, res );

                } catch( const exception &ex ) {
                    res = HTTPRequestHandler::HTTP_INTERNAL_SERVER_ERROR;
                    sendHttpError( req, res );
                }
            }

            if( !persist && req.getVersion() <= 1.1 )
                req.set( "Connection", "close" );

            req.sendHTTPHeaders( res );
        }
	} catch( const Exception &ex ) {
		cerr << ex.toString() << endl;
	} catch( const exception &ex ) {
		cerr << ex.what() << endl;
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
FieldStorage::FieldStorage( istream &is, bool bKeepBlanks )
{
	string sName, sValue;
	bool bFetchValue = false;

	while(!is.eof()) {
		int ch;
		if((ch = is.get()) == -1 || ch == '\n' )
			break;

		if( ch == '&' ) { // add values
			if( bKeepBlanks || !sValue.empty())
				m_fields.push_back( pair<string,string>( sName, sValue ));

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
		if( bKeepBlanks || !sValue.empty()) {
			// cerr << "url : " << sName << " + " << sValue << endl;
			m_fields.push_back( pair<string, string>( sName, sValue ));
		}

	if(m_fields.empty())
		throw invalid_argument( "Stream does'nt contain any valid data" );
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

	os << endl;
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

HTTPRequestHandler::Result HTTPFileHandler::handle( HTTPRequest &req )
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
			req.set( "Content-Type", getMime( sExt ));

		if( req.sendFile( sPath ))
			return HTTP_OK;
	}

	return HTTP_NOT_FOUND;
}
