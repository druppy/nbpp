/**
   Impliment a simple HTTP 1.0 server, to make the base for simple
   web services. This IS NOT a new apache, but just a simple tool
   to make use of the HTTP protocol, for applications.

   The idea is to keep it simple, but using real threads as any other
   object in nb++.

   RFC 1945 is the base document for this service, so no socket
   keepalive or multi parts ... yet.
*/
#ifndef _HTTPSERVER_HPP
#define _HTTPSERVER_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <nb++/Url.hpp>
#include <nb++/NetworkDaemon.hpp>
#include <nb++/InetAddress.hpp>
#include <nb++/ServerSocket.hpp>

namespace nbpp {

	using namespace std;

	class HTTPRequest;

	/**
	   To handle a HTTP request this class must be implimented, and added to a
	   running server. When a request arives at the server, it will be directed
	   to this handler if its related filter is fullfiled.

	   If more than one request occures, this class will be called reentrant.

	   When the Request ends, it will make sure to set a propper "Content-Lenght"
	   and send all data to the client.
	 */
	class HTTPRequestHandler : public RefCounted
	{
	public:
        // If true the http server will leave the socket to this handler in order to finish it
        // This is useful for long polls or even web sockets, default is false
        
		enum Result {
            HTTP_CONTINUE = 100,
            HTTP_SWITCHING_PROTOCOLS = 101,
            HTTP_OK = 200,
            HTTP_CREATED = 201,
            HTTP_ACCEPTED = 202,
            HTTP_NO_CONTENT = 204,
            HTTP_RESET_CONTENT = 205,
            HTTP_PARTIAL_CONTENT = 206,
            HTTP_MOVED_PERMANENTLY = 301,
            HTTP_MOVED_TEMPORARILY = 302,
            HTTP_NOT_MODIFIED = 304,
            HTTP_USE_PROXY = 305,
            HTTP_TEMPORARY_REDIRECT = 307,
            HTTP_BAD_REQUEST = 400,
            HTTP_UNAUTHORIZED = 401,
            HTTP_FORBIDDEN = 403,
            HTTP_NOT_FOUND = 404,
            HTTP_METHOD_NOT_ALLOWED = 405,
            HTTP_NOT_ACCEPTABLE = 406,
            HTTP_PROXY_AUTHENTICATION_REQUIRED = 407,
            HTTP_REQUEST_TIMEOUT = 408,
            HTTP_CONFLICT = 409,
            HTTP_GONE = 410,
            HTTP_LENGTH_REQUIRED = 411,
            HTTP_PRECONDITION_FAILED = 412,
            HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
            HTTP_REQUEST_URI_TOO_LONG = 414,
            HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
            HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
            HTTP_EXPECTATION_FAILED = 417,
            HTTP_INTERNAL_SERVER_ERROR = 500,
            HTTP_NOT_IMPLEMENTED = 501,
            HTTP_BAD_GATEWAY = 502,
            HTTP_SERVICE_UNAVAILABLE =503,
            HTTP_GATEWAY_TIMEOUT = 504,
            HTTP_VERSION_NOT_SUPPORTED = 505
		};

        HTTPRequestHandler() {}

		virtual Result handle( HTTPRequest &req ) = 0;
	};

	/**
	   This request class will be send to the handler every time there arives
	   a nev HTTP request.

	   This class handle the flow of the HTTP protecol, by parsing the incomming
	   header, and the parse over the content (via the istream). The responce the
	   the other end is send as the header and the ostream.

	   There is no content parsing in this class
	 */
	class HTTPRequest {
	public:
		struct NameValue{
			NameValue( const string &name, const string &value ) :
				sName( name ), sValue( value ) {}

			bool operator==( const string &name ) const {return sName == name;}
			bool operator!=( const string &name ) const {return sName != name;}

			string sName;
			string sValue;
		};
		typedef vector<NameValue> values_t;

		HTTPRequest( Socket &socket );
		~HTTPRequest();

		/**
		   Get a stream to read the body of the request, if needed.

		   @return istream the input stream.
		 */
		istream &getInputStream();

		/**
		   Get the output stream, for the responce for this request. When this
		   function have been called, the "set" function will not be callable
		   as the header of the request already have been send by calling this
		   function.

		   This will also be done explicitly by sendHttpHeader.
		 */
		ostream &getOutputStream();

        size_t getOutStreamSize() const { return m_os.str().size(); }

		enum Method {
			UNKNOWN,
			GET,
			HEAD,
            PUT,
			POST,
            DELETE
		};

		/**
		   Get the client socket
		 */
		Socket& getSocket( void );

		/**
		   That is the request method.
		 */
		Method getMethod( void ) const;

		/**
		   Get the full URL as the client requested it.
		 */
		const URL &getUrl( void ) const;

		/**
		   Get the HTTP version number as a float.

		   @return the float value 0.9, 1.0 or 1.1 as for now.
		 */
		float getVersion( void ) const {return m_nVersion;}

		/**
		   Check if a header field is presend.

		   @param sName the name of the field
		   @return true if found else false.
		 */
		bool hasA( const string &sName ) const;

		/**
		   Return a input header ellement, by a given name.

		   @exception invalid_argument if name is not found.
		 */
		string operator[]( const string &sName ) const;

        /**
            Get a list of all input headers
        */
        values_t headers_in() {return m_header_in;}

		/**
		   Set a HTTP header ellement, for the responce for this element.

		   This can only be done before calling the sendHttpHeaders or the
		   getOutputStream. If called after that an exeption is rised.

		   If the header id is already set it will be overwritten by this
		   call. This can be suppressed by setting replace parameter to false

		   @param sName is the name of the header element.
		   @param sValue is the value of the ellement.
                   @param sReplace should the new header replace existing with same name
		 */
		void set( const string &sName, const string &sValue, bool sReplace = true );

        /**
           Set header elemenet, but convert it from a from a integer.

           @see set
        */
        void set( const string &name, size_t val, bool replace = true );

		/**
		   This will send all headers to the HTTP client, and make the request
		   ready for input data via the cout stream.

		   This will be done implicit by the first use of the HTTPOutStream.

		   This can only be done ones, in a request.
		 */
		void sendHTTPHeaders( HTTPRequestHandler::Result res = HTTPRequestHandler::HTTP_OK );

		/**
		   To send large files, please use this function to reduce the amount of memory
		   used for this action.

		   The file must be a file system file, that have a known and static size !

		   @param sFname The name of the file
           @param offset offset of file to send in partial file transfer
           @param length the length of the file chunck to send (0 = all)
		   @return true if send, else false if the file does not exist
		 */
		bool sendFile( const string &sFname, size_t offset, size_t length = 0 );

        bool sendFile( const string &sFname);

        void takeover() {_takeover = true;}
        bool has_takeover() const {return _takeover;}
	private:
		float m_nVersion;

		URL m_url;
		values_t m_header_in,
			m_header_out;
		bool m_bHeaderSend;
		Socket m_socket;
		ostringstream m_os;
		Method m_method;
        bool _takeover;   // Tell the http handler to drop handling this request
	};

	/**
	   A commen usage of a HTTP server is the ability to serve files. This
	   class will come in handy if there is a need to serve static content
	   from the filesystem, together with the other user handlers.

	   There is no fancy suid things here, we are only able the se files as
	   shown the user that starts us can. Not caching is done either.
	 */
	class HTTPFileHandler : public HTTPRequestHandler {
	public:
		/**
		   Create a new HTTPFileHandler that maps the sUriPath to the sFilePath,
		   like this :

		   If sUriPath = "/documents" and sFilePath = "/home/user/doc", the request
		   for "/documents/index.html" to our server will end up as the file
		   "/home/user/doc/index.html" and the content of this will be send back
		   as this.

		   So both defines the root of the file tree, and all added to the URI path
		   will be added to the file path too.

		   @param sUrlPath The path as requested by the client
		   @param sFilePath The path in the file system.
		 */
		HTTPFileHandler( const string &sUriPath, const string &sFilePath );

		/**
		   Map a file name extention to a mime type, for the HTTP response
		   codes.

		   @param sExt The file extention ("html")
		   @param sMime the mime identifier ("text/html")
		 */
		void addMime( const string &sExt, const string &sMime );

		/**
		   Do something about it.
		 */
		Result handle( HTTPRequest &req );
	private:
		string getMime( const string &sExt ) const;

		string m_sUrlPath,
			m_sFilePath;
		typedef vector< pair<string, string> > exts_t;
		exts_t m_exts;
	};

	/**
	   Contain a set of path, file extentions and mime types, that will
	   invoke a related handle.
	 */
	class HTTPFilter {
		typedef vector<string> Strings;
		Strings m_paths,
			m_exts,
			m_mime;
	public:
		bool match( const URL &sUrl ) const;

		void addPath( const string &sPath ) {m_paths.push_back( sPath );}
		void addExt( const string &sFileExt ) {m_exts.push_back( sFileExt );}
		void addMime( const string &sMime ) {m_mime.push_back( sMime );}
	};

	/**
	   The main server class for the nb++ HTTP server. Make one instance of
	   this and start is as a thread. On every request the server will match
	   the request path, and will select the first awailable handler for the
	   request or retuen HTTP_NOT_FOUND.

	   The server itself maintain a threadpool, and a handler will therefor
	   need to be reentrant, as mode than one thread may call it.
	 */
	class HTTPServer : public NetworkDaemon<InetAddress> {
		struct Handler {
			Handler( const HTTPFilter &f, HTTPRequestHandler *pHndl ) :
				filter( f ), hndl( pHndl ) {}
			HTTPFilter filter;
			RefHandle<HTTPRequestHandler> hndl;
		};
		typedef vector<Handler> handlers_t;
		handlers_t m_handlers;
		InetAddress m_inetAddr;   // The server inet address
	public:
		HTTPServer( const string &sPrgName );
		~HTTPServer() throw() {}
		void addHandler( HTTPFilter &filter, HTTPRequestHandler *pHndl );

		void setInetAddress( InetAddress iadr );
	protected:
		// This may be called reentrant
		void handleConnection( NetworkConnection<InetAddress> &connection )
			throw( exception, Exception );

		virtual ServerSocket initServerSocket( void ) throw( exception, Exception );

		void reload( void ) throw (std::exception, nbpp::Exception) {}
	};

    /**
	   Defines the a class that is able to parse the RFC1738, and put the elements
	   in a accessable form.

	   The class is able to read, and write data in the RFC 1738 format, to a std. C++
	   stream.
	*/
	class FieldStorage {
	public:
		FieldStorage( void ) {}
		FieldStorage( istream &is, bool bKeepBlanks = false );

		void append( const string &sName, const string &sData );
		void append( const string &sName, long nData );

		bool hasA( const string &sName ) const;
		string operator [](const string &sName) const;
		string operator []( unsigned idx ) const;

		unsigned size( void ) const {return m_fields.size();}

		string toString( void ) const;
		void out( ostream &os ) const;
	private:
		typedef vector<pair<string, string> > vpdata_t;
		string conv_data( const string &str ) const;
		vpdata_t m_fields;
	};
};

#endif /* _HTTPSERVER_HPP */
