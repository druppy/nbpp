/**
   The URL class and some helper classes
*/

#include <nb++/Url.hpp>

#include <nb++/Socket.hpp>
#include <nb++/InetAddress.hpp>

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace nbpp;

/**
   Find the propper tokens and unescape symbols
*/
class UrlLex {
public:
	enum Type {
		tEof,
		tReserved, 	// ";" | "/" | "?" | ":" | "@" | "&" | "=" | "$" | ","
		tSymbol
	};

	UrlLex( const string &str ) : m_str( str ) {
		m_iter = m_str.begin();
		m_type = tEof;
		setReserved();
	}

	void setReserved( const string &sReserved = ";/?:@&=" ) {
		m_sReserved = sReserved;
	}

	Type next( void ) {
		if( m_iter == m_str.end()) {
			m_type = tEof;
			m_sToken = "";
			return m_type;
		}

		if( strchr( m_sReserved.c_str(), *m_iter )) {
			m_sToken = *m_iter++;
			return m_type = tReserved;
		}

		if( isalnum( *m_iter ) || strchr( "$-._%~+", *m_iter )) {
			string sRes;
			do {
				if( *m_iter == '%' ) {
					char szHex[ 3 ];
					m_iter++;
					if( m_iter == m_str.end()) break;

					szHex[ 0 ] = *m_iter++;
					if( m_iter == m_str.end()) break;
					szHex[ 1 ] = *m_iter++;
					szHex[ 2 ] = 0;

					sRes += (char)strtol( szHex, NULL, 16 );
				}
				else {
                    if( *m_iter == '+' )
                        sRes += ' ';
                    else
                        sRes += *m_iter;
                    ++m_iter;
                }

			} while( m_iter != m_str.end() && !strchr( m_sReserved.c_str(), *m_iter ));
			m_sToken = sRes;
			return m_type = tSymbol;
		}

		return m_type = tEof;
	}

	string getToken( void ) const {return m_sToken;}
	Type getType( void ) const {return m_type;}
private:
	string m_str, m_sToken, m_sReserved;
	string::iterator m_iter;
	Type m_type;
};

static string url_escape( const string &str )
{
	string res;
	string::const_iterator i;

	for( i = str.begin(); i != str.end(); i++ ) {
		if( isalnum( *i ) || *i == '_' )
			res += *i;
		else {
            if( *i == ' ' )
                res += '+';
            else {
                char szBuf[ 5 ];
                sprintf( szBuf, "%%%02X", (unsigned char)*i );
                res += szBuf;
            }
		}
	}
	return res;
}

string nbpp::string_tolower( const string &str )
{
	string res;
	res.reserve( str.length());
	string::const_iterator i = str.begin();
	while( i != str.end())
		res += (char)tolower( *(i++) );

	return res;
}

using namespace nbpp;

////////////////////////////////
// Impl. of URLException

URLException::URLException( const string &sError ) : Exception( sError )
{
}

URLException::~URLException() throw()
{
}

///////////////////////////////
// Impl. of class Url

URL::URL( void )
{
	m_nPort = -1;
}

URL::URL( const string &sUrl )
{
	set( sUrl );
}

void URL::set( const string &sUrl )
{
	UrlLex lex( sUrl );

	clear();
	m_sFragment = m_sScheme = m_sAuth = m_sHost = m_sPath = "";
	m_nPort = -1;

	// Fetch the scheme, if any
	if( UrlLex::tSymbol == lex.next()) {
		string sScheme = lex.getToken();
		if( lex.next() == UrlLex::tReserved && lex.getToken() == ":" ) {
			if( lex.next() == UrlLex::tReserved && lex.getToken() == "/" ) {
				if( lex.next() == UrlLex::tReserved && lex.getToken() == "/" ) {
					m_sScheme = sScheme;
					lex.next();
				}
				else
					throw URLException( "Badly formed scheme name in " + sUrl );
			}
			else
				throw URLException( "Badly formed scheme name in " + sUrl );
		}
	}

	// Get the host name, auth and port, is any
	if( lex.getType() == UrlLex::tSymbol ) {
		string sName = lex.getToken();

		if( lex.next() == UrlLex::tReserved ) {
			if( lex.getToken() == "@" ) {
				m_sAuth = sName;
				if( lex.next() == UrlLex::tSymbol )
					m_sHost = lex.getToken();
				else
					throw URLException( "Badly formed host name " + sUrl );

				lex.next();
			}

			if( lex.getToken() == ":" ) {
				if( m_sHost.empty())
					m_sHost = sName;

				if( lex.next() == UrlLex::tSymbol )
					m_nPort = atoi( lex.getToken().c_str());

				lex.next();
			}

			if( m_sHost.empty()) {
				if( lex.getToken() == "/" )
					m_sHost = sName;
				else
					throw URLException( "Missing host name " + sUrl );
			}
		}
		else
			m_sHost = sName;
	}

	// Get path info, if any
	if( lex.getType() == UrlLex::tReserved && lex.getToken() == "/" ) {
		m_sPath = "/";
		lex.next();

		do {
			if( lex.getType() == UrlLex::tSymbol ) {
				m_sPath += lex.getToken();
				if( lex.next() == UrlLex::tReserved && lex.getToken() == "/" ) {
					m_sPath += "/";
					lex.next();
				}
			}
			else
				break;
		} while( lex.getType() != UrlLex::tEof );
	}

	// Get fragment, if any
	if( lex.getType() == UrlLex::tReserved && lex.getToken() == "#" ) {
		lex.setReserved("");
		if( lex.next() == UrlLex::tSymbol )
			m_sFragment = lex.getToken();
	}

	// Get options / fragment / ancher
    if( lex.getType() == UrlLex::tReserved && lex.getToken() == "?" ) {
        string sOptionName, sOptionValue;
        do {
            if( lex.next() == UrlLex::tSymbol )
                sOptionName = lex.getToken();

            sOptionValue.clear();

            if( lex.next() == UrlLex::tReserved && lex.getToken() == "=" ) {
                if( lex.next() == UrlLex::tSymbol ) {
                    sOptionValue = lex.getToken();
                    lex.next();
                }
            } else { // it may not be a key value pair
                sOptionValue = sOptionName;
                sOptionName.clear();
            }

            push_back( make_pair( sOptionName, sOptionValue ));

            if( lex.getType() != UrlLex::tReserved || lex.getToken() != "&" )
                break;

        } while( lex.getType() != UrlLex::tEof );

        // More fragment !
        if( lex.getToken() == "#" ) {
            lex.setReserved( "" );
            if( lex.next() == UrlLex::tSymbol )
                m_sFragment = lex.getToken();
        }
    } else {
        lex.setReserved( "" );
        if( lex.next() == UrlLex::tSymbol )
            m_sFragment = lex.getToken();
    }
}

string URL::getUri( void ) const
{
	ostringstream os;
	os << m_sPath;

	if( size() ) {
		os << '?';
		URL::const_iterator i;
		for( i = begin(); i != end(); i++ ) {
			if( i != begin())
				os << '&';

			os << url_escape( i->first ) << "=" << url_escape( i->second );
		}
	}
	return os.str();
}

void URL::add( const string &sName, const string &sValue )
{
	push_back( make_pair( sName, sValue ));
}

bool URL::hasA( const string &sName ) const
{
	vector<pair<string,string> >::const_iterator i;

	for( i = begin(); i != end(); i++ )
		if( i->first == sName )
			return true;

	return false;
}

string URL::operator[]( const string &sName ) const
{
	vector<pair<string,string> >::const_iterator i;

	for( i = begin(); i != end(); i++ )
		if( i->first == sName )
			return  i->second;

	throw invalid_argument( "Unknown option " + sName + "on URL" );
}

string URL::toString( void ) const
{
	ostringstream os;

	if( !m_sScheme.empty())
		os << m_sScheme << "://";

	if( !m_sAuth.empty())
		os << m_sAuth << "@";

	if( !m_sHost.empty())
		os << m_sHost;

	if( m_nPort != -1 )
		os << ":" << m_nPort;

	os << getUri();

	if( !m_sFragment.empty() )
		os << "#" << m_sFragment;

	return os.str();
}

URLConnectionHndl URL::getConnection( void )
{
	return URLSchemes::getInstance()[ *this ];
}

////////////////////////
// Impl. of URLSchemes

static URLSchemes *g_pSchemes = NULL;

URLSchemes &URLSchemes::getInstance()
{
	if( !g_pSchemes )
		g_pSchemes = new URLSchemes;

	return *g_pSchemes;
}

bool URLSchemes::hasA( const string &sScheme ) const
{
	return m_connections.count( sScheme ) > 0;
}

URLConnectionHndl URLSchemes::operator[]( const URL &url )
{
	connections_t::const_iterator iter = m_connections.find( url.getSchemeType() );

	if( iter != m_connections.end())
		return iter->second->makeConnection( url );

	throw invalid_argument( url.toString() + " has not a implimented scheme" );
}

///////////////////////
// Impl. of URLScheme

bool URLConnection::hasA( const string &sName ) const
{
	string sLowName = string_tolower( sName );

	options_t::const_iterator iter = m_in_header.begin();

	for(; iter != m_in_header.end(); iter++ )
		if( iter->first == sLowName )
			return true;

	return false;
}

string URLConnection::operator[]( const string &sName ) const
{
	string sLowName = string_tolower( sName );

	options_t::const_iterator iter = m_in_header.begin();
	for(; iter != m_in_header.end(); iter++ )
		if( iter->first == sLowName )
			return iter->second;

	throw invalid_argument( sName + " is not an option" );
}

/////////////////////////////
// Impl. of the file scheme

class FileConnection : public URLConnection
{
	ostream *m_pOutputStream;
	ifstream m_istream;
	stringstream m_sstream;
public:
	FileConnection( const URL &url );
	virtual ~FileConnection() {}

	void setTimeout( unsigned nTimeout ) {
		throw invalid_argument( "Can't set timeout on a file" );
	}
	void set( const string &sName, const string &sValue ) {} // Just ignore it

	void perform( istream &inputStream, size_t nLength );
	void perform( const string &sPostData = "" );

	ostream &getOutputStream( void );
	void setOutputStream( ostream &is );
};

FileConnection::FileConnection( const URL &url ) : URLConnection( url )
{
	m_pOutputStream = NULL;
}

void FileConnection::perform( istream &inputStream, size_t nLength )
{
	string sFname( m_url.getHost() + m_url.getPath());
	ofstream os( sFname.c_str() );

	if( !inputStream.eof())
		os << inputStream.rdbuf();
}

void FileConnection::perform( const string &sPostData )
{
	string sFname( m_url.getHost() + m_url.getPath());
	if( !sPostData.empty() ) {
		ofstream os( sFname.c_str());

		os << sPostData;
	}
	else {
		m_istream.open( sFname.c_str());

		if( m_pOutputStream ) {
			*m_pOutputStream << m_istream.rdbuf();

			m_pOutputStream = NULL;
		}
		else
			m_sstream <<  m_istream.rdbuf();
	}
}

void FileConnection::setOutputStream( ostream &is )
{
	m_pOutputStream = &is;
}

ostream &FileConnection::getOutputStream( void )
{
	if( m_pOutputStream )
		return *m_pOutputStream;

	return m_sstream;
}

class FileConnectionAdder
{
public:
	FileConnectionAdder( void ) {
		URLSchemes &schemes = URLSchemes::getInstance();

		schemes.add( "file", new URLConnectionMaker<FileConnection>());
	}
};

static FileConnectionAdder adder;

//////////////////////////
// Impl. HTTP URL Scheme

#ifndef NBPP_HAVE_CURL_CURL_H

static bool split_head( const string &sLine, string &sName, string &sValue )
{
	string::size_type pos = sLine.find( ':' );

	if( pos == string::npos )
		return false;

	sName = sLine.substr( 0, pos );
	sValue = sLine.substr( pos + 2, string::npos );

	return true;
}

class HTTPConnection : public URLConnection
{
	URL m_url;
	options_t m_out_headers;
	Socket m_sock;

	unsigned m_nTimeout;
	ostream *m_pStream;
	stringstream m_result;
public:
	HTTPConnection( const URL &url );
	~HTTPConnection();

	void setTimeout( unsigned nTimeout ) {m_nTimeout = nTimeout;}
	void set( const string &sName, const string &sValue );

	void perform( istream &inputStream, size_t nLength );
	void perform( const string &sPostData );

	ostream &getOutputStream( void );
	void setOutputStream( ostream &is );
};

HTTPConnection::HTTPConnection( const URL &url ) : URLConnection( url ),
	m_sock( InetAddress::getByName( url.getHost(), url.getPort()), 10 )
{
	m_pStream = &m_result;
}

HTTPConnection::~HTTPConnection()
{
}

void HTTPConnection::set( const string &sName, const string &sValue )
{
	m_out_headers.push_back( make_pair( sName, sValue ));
}

void HTTPConnection::perform( const string &sPostData = "" )
{
}

void HTTPConnection::perform( istream &inputStream, size_t nLength )
{
	ostream &os = m_sock.getOutputStream();

	os << "GET " << m_url.getPath() << "HTTP/1.1\r\n";

	options_t::const_iterator iter;
	for( iter = m_out_headers.begin(); iter != m_out_headers.end(); iter++ )
		os << iter->first << ": " << iter->second << "\r\n";

	// Here we need some istream data !
	if( inputStream ) {
		os << "Content-Length: " << nLength << "\r\n";
		os << "\r\n";

		while( nLength > 0 ) {
			char sBuffer[ 1024 ];

			unsigned nLen = ( nLength < (long)sizeof( sBuffer )) ? nLength : sizeof( sBuffer );

			inputStream.read( sBuffer, nLen );
			nLen = inputStream.gcount();
			os.write( sBuffer, nLen );

			nLength -= nLen;
		}
		os << "\r\n";
	}

	os << "\r\n";
	os.flush();

	m_sock.waitForInput();
	istream &is = m_sock.getInputStream();

	if( is.get() == 'H' ) {
		float nVersion;
		unsigned nStatus;
		char szDesc[ 256 ];
		string sLine;

		getline( is, sLine );
		if( 3 != sscanf( sLine.c_str(), "HTTP/%f %u %255[^\r]", &nVersion, &nStatus, szDesc ))
			throw invalid_argument( "Error in HTTP request, bad status format." );

		do {
			sLine.erase();

			if( m_sock.checkForInput()) {
				string sName, sValue;

				getline( is, sLine );
				if( split_head( sLine, sName, sValue ))
					m_in_header.push_back( make_pair( sName, sValue ));
			}
		} while( !sLine.empty());
	}
	else
		is.unget();

	// Get HTTP 1,x content data
	if( hasA( "Content-Length" )) {
		unsigned nSize = atoi( (*this)[ "Content-Length" ].c_str());
		istream &is = m_sock.getInputStream();
		while( nSize > 0 ) {
			if( m_sock.checkForInput()) {
				char sBuffer[ 1024 ];

				is.read( sBuffer, sizeof( sBuffer ));
				unsigned nLen = is.gcount();
				m_pStream->write( sBuffer, nLen );

				nSize -= nLen;
			}
		}
	}

	m_pStream = &m_result;
}

ostream &HTTPConnection::getOutputStream( void )
{
	return *m_pStream;
}

void HTTPConnection::setOutputStream( ostream &os )
{
	m_pStream = &os;
}

/////////////////////////
// Impl. file URL scheme

class ConnectionAdder
{
public:
	ConnectionAdder( void ) {
		URLSchemes &schemes = URLSchemes::getInstance();

		schemes.add( "http", new URLConnectionMaker<HTTPConnection>());
	}
};

static ConnectionAdder conn_adder;

#endif
