/**
   Make a curl wrapper for the nb++ Url class.
*/
#include <nb++/Url.hpp>

#include <sstream>
#include <stdexcept>

#ifdef NBPP_HAVE_CURL_CURL_H

#include <curl/curl.h>

using namespace nbpp;

/**
   The basic class for all Curl connection, that takes care of propper
   class init and some internal stream setup.
*/
class CurlConnection : public URLConnection
{
	CURL *m_curl;
	curl_slist *m_pHeader;
	ostream *m_pOutputStream;
	stringstream m_result;
	char m_szError[ CURL_ERROR_SIZE + 1 ];
        char m_margin[1024*16];
protected:
	void setopt( CURLoption curl_option, void *curl_data );
	void setopt( CURLoption curl_option, long curl_data );

public:
	CurlConnection( const URL &url );
	virtual ~CurlConnection();

	virtual void perform( istream &inputStream, size_t nLen );
	virtual void perform( const string &sPostData = "");

	void set( const string &sName, const string &sValue );
	void setTimeout( unsigned nUsec );
	ostream &getOutputStream( void );
	void setOutputStream( ostream &os );
};

/**
   The pure http version of the curl connection scheme. Its basicly just
   a thin wrapper, of the CurlConnection class, as the basic of Curl is
   http.
*/
class CurlHttpConnection : public CurlConnection
{
public:
	CurlHttpConnection( const URL &url );

	// void perform( istream *pInputStream, long nLen );
};

/**
   The http client code, sof simple https connections
*/
class CurlHttpsConnection : public CurlConnection
{
public:
	CurlHttpsConnection( const URL &url );

	// void perform( istream *pInputStream, long nLen );
};

class CurlFtpConnection : public CurlConnection
{
public:
	CurlFtpConnection( const URL &url );

	// void perform( istream *pInputStream, long nLen );
};

/////////////////////////////////
// Impl. of some common C funcs.

static void curl_global_init( void )
{
	static bool hasInit = false;

	if( !hasInit ) {
		curl_global_init( CURL_GLOBAL_ALL );
		hasInit = true;
	}
}

size_t stream_reader( void *ptr, size_t size, size_t nmemb, void *data )
{
	istream *pStream = static_cast<istream *>( data );

	pStream->read((char *)ptr, size * nmemb );

	// string tmp( (char *)ptr, pStream->gcount() );

	// cout << "Read : (" << nmemb*size << ") " << tmp << endl;

	// cout << "Count " << pStream->gcount() << endl;

	return pStream->gcount();
}

size_t stream_writer( void *ptr, size_t size, size_t nmemb, void *data )
{
	// string tmp( (char *)ptr, nmemb * size );

	// cout << "Write : " << tmp << endl;

	ostream *pStream = static_cast<ostream *>( data );

	pStream->write((char *)ptr, size * nmemb );

	return size * nmemb;
}

size_t header_writer( void *ptr, size_t size, size_t nmemb, void *data )
{
	options_t *pOptions = static_cast<options_t *>( data );

	string sTmp((char *)ptr, size * nmemb );

	string::size_type pos = sTmp.find( ':' );

	string sName = string_tolower( sTmp.substr( 0, pos ));

	pOptions->push_back( make_pair( sName, sTmp.substr( pos + 2, string::npos )));

	return size * nmemb;
}

////////////////////////////
// Impl. of CurlConnection

void CurlConnection::setopt( CURLoption curl_option, void *curl_data )
{
 	CURLcode code;
	code = curl_easy_setopt( m_curl, curl_option, curl_data );

 	if( code != CURLE_OK )
		throw invalid_argument( m_szError );
}

void CurlConnection::setopt( CURLoption curl_option, long curl_data )
{
	CURLcode code;
	code = curl_easy_setopt( m_curl, curl_option, curl_data);

	if( code != CURLE_OK )
		throw invalid_argument( m_szError );
}

CurlConnection::CurlConnection( const URL &url ) : URLConnection( url )
{
	memset( m_margin,  0, sizeof( m_margin ));

	curl_global_init();

	m_pHeader = NULL;

	m_curl = curl_easy_init();

	setopt( CURLOPT_ERRORBUFFER, m_szError );
	m_szError[0] = 0;
	
	setopt( CURLOPT_WRITEFUNCTION, (void *)stream_writer );
	setopt( CURLOPT_HEADERFUNCTION, (void *)header_writer );
	setopt( CURLOPT_READFUNCTION, (void *)stream_reader );

	// setopt( CURLOPT_DNS_CACHE_TIMEOUT, (long)0 );     // Disable dns cacheing
	setopt( CURLOPT_DNS_USE_GLOBAL_CACHE, (long)0 );  // This is not thread safe
	setopt( CURLOPT_FAILONERROR, (long)0 );
	setopt( CURLOPT_SSL_VERIFYPEER, (long)0 );  // Don't verify SSL
	setopt( CURLOPT_SSL_VERIFYHOST, (long)2 );  // Its OK not to dif. host name
	setopt( CURLOPT_NOPROGRESS, 1 );
	setopt( CURLOPT_NOSIGNAL, 1 );
	setopt( CURLOPT_FOLLOWLOCATION, 1 );        // Follow locations if given

	m_pOutputStream = &m_result; // Store data in the result stringstream by default
}

CurlConnection::~CurlConnection()
{
	curl_easy_cleanup( m_curl );

	if( m_pHeader )
		curl_slist_free_all( m_pHeader );

        if(m_margin[0]){
	   cerr << "~CurlConnection m_margin not left clean !!!!!!!!!!!: " << m_margin << endl;
        }
}

void CurlConnection::setTimeout( unsigned nUsec )
{
	long sec = nUsec / 1000;
	sec = (sec == 0) ? 1 : sec;
	setopt( CURLOPT_CONNECTTIMEOUT, sec );
	setopt( CURLOPT_TIMEOUT, sec );
	cerr << "Setting curl timeout : " << sec << endl;
}

ostream &CurlConnection::getOutputStream( void )
{
	return *m_pOutputStream;
}

void CurlConnection::setOutputStream( ostream &os )
{
	m_pOutputStream = &os;
}

void CurlConnection::set( const string &sName, const string &sValue )
{
	string sTmp = sName + ": " + sValue;

	m_pHeader = curl_slist_append( m_pHeader, sTmp.c_str() );
}

void CurlConnection::perform( const string &sPostData )
{
	if( !m_in_header.empty())
		m_in_header.clear();

	string sUrl = m_url.toString();

	setopt( CURLOPT_URL, (void *)sUrl.c_str());

	setopt( CURLOPT_WRITEDATA, (void *)m_pOutputStream );
	setopt( CURLOPT_WRITEHEADER, (void *)&m_in_header );

	// Need to post data ?
	if( !sPostData.empty() ) {
		setopt( CURLOPT_POSTFIELDS, (void *)sPostData.c_str() );
		setopt( CURLOPT_POSTFIELDSIZE, sPostData.length() );
		setopt( CURLOPT_POST, 1 );
	}

	// Well, data are to be found on the url
	if( m_pHeader )
		setopt( CURLOPT_HTTPHEADER, (void *)m_pHeader );

	// Where to put the result !

 	CURLcode res = curl_easy_perform( m_curl );

	m_pOutputStream = &m_result; // Reset the custom output stream

	if( res != CURLE_OK ) {
		if( res == CURLE_OPERATION_TIMEOUTED )
			throw TimeoutException( m_szError );

		if( strlen( m_szError ) == 0 )
			throw ConnectException( curl_easy_strerror(res));
		else
			throw ConnectException( m_szError );
	} else {
    	long response_code = 0;
    	curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);

    	// Note that the output buffer now contain the error body !
    	if( response_code > 200 ) {
    		stringstream os;
    		
    		os << "HTTP error : " << response_code;

    		throw ConnectException( (int)response_code, os.str() );
    	}
	}
}

void CurlConnection::perform( istream &inputStream, size_t nLen )
{
	setopt( CURLOPT_UPLOAD, 1 );  // Implicit PUT
	// Activate stream wrapper !
	setopt( CURLOPT_READDATA, (void *)&inputStream );
	setopt( CURLOPT_INFILESIZE, nLen );

	perform();
}

/////////////////////////////////
// Impl. of CurlHttpConnection

CurlHttpConnection::CurlHttpConnection( const URL &url ) : CurlConnection( url )
{
}

/////////////////////////////////
// Impl of CurlHttpsConneciton

CurlHttpsConnection::CurlHttpsConnection( const URL &url ) : CurlConnection( url )
{
}

///////////////////////////////////
// Impl. of CurlFtlConnection

CurlFtpConnection::CurlFtpConnection( const URL &url ) : CurlConnection( url )
{
}

// Add it all to the scheme container

class CurlSchemeAdder
{
public:
	CurlSchemeAdder( void ) {
		URLSchemes &schemes = URLSchemes::getInstance();

		schemes.add( "http", new URLConnectionMaker<CurlHttpConnection>());
		schemes.add( "https", new URLConnectionMaker<CurlHttpsConnection>());
		schemes.add( "ftp", new URLConnectionMaker<CurlFtpConnection>());
	}
};

static CurlSchemeAdder adder;

#endif
