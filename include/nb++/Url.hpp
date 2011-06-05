/**
	Url.hpp defines a class that make access to a stream of data, 
	easy and painless, by hidding the protocol implimentation.

	It's the idea that it should both be easy to use and easy to 
	add new methods, based on this mini framework.

	Together with the HTTPserver this should hopefully make a sound WEB 
	foundation for all future nb++ applications.

	The Url definition is to be found in RFC 1738
*/
#ifndef _URL_HPP
#define _URL_HPP

#include <nb++/RefCount.hpp>
#include <nb++/Exception.hpp>

#include <map>
#include <vector>
#include <string>

namespace nbpp
{
	using namespace std;

	class URLConnection;
	typedef RefHandle<URLConnection> URLConnectionHndl;

	/**
	 	Base URL class to make it possible to access complex protocols like http, ftp
		and other, in a simple and homogene manner.

		It derive from a vector of string pairs, that is the options for the URL in
		an unencoded version.

		It is possible to both parse a URL string and to construct a new one, and if 
		the scheme of the URL is known, it is possible to create a connection to the 
		URL.
	 */
	class URL : public vector<pair<string, string> > {
	public:
		/// Construct an empty URL class
		URL( void );

		/// Contruct using another URL
		URL( const string &sUrl );
	
		/**
		   Set a new URL, and reset what ever inside.

		   @exception URLException if problems with the syntax
		   @param sUrl the new url
		*/
		void set( const string &sUrl );

		/// Get the scheme type like http, https, ftp etc.
		string getSchemeType( void ) const {return m_sScheme;}

		/// Get autority string
		string getAuthority( void ) const {return m_sAuth;}

		/// Get the path of the url
		string getPath( void ) const {return m_sPath;}

		/// Get the host name
		string getHost( void ) const {return m_sHost;}

		/// Get the port number
		int getPort( void ) const {return m_nPort;}

		/// Get the url fragment
		string getFragment( void ) const {return m_sFragment;}
		
		/// add a new option
		void add( const string &sName, const string &sValue );

		/// Check to se if an option exsist
		bool hasA( const string &sName ) const;

		/// Retrive the value of the option
		string operator[]( const string &sName ) const;

		/// Get the URI part of the URL, to use by HTTP servers
		string getUri( void ) const;

		/// Get the full URL, by reconstruction it from current data
		string toString( void ) const;

		/// Set the new Scheme type name
		void setSchemeType( const string &sScheme )     {m_sScheme = sScheme;}

		/// Set a new authorize string
		void setAuthority( const string &sAuth )    {m_sAuth = sAuth;}

		/// Set a new path
		void setPath( const string &sPath )         {m_sPath = sPath;}

		/// Set a new host name
		void setHost( const string &sHost )         {m_sHost = sHost;}

		/// set a new port number
		void setPort( int nPort )                   {m_nPort = nPort;}

		/// Set a new fragment string
		void setFragment( const string &sFragment ) {m_sFragment = sFragment;}

		/**
		   Get a new connection for this URL, to enable communication using
		   this URL. The communication will last as long as the user hold on 
		   to the handle, as it is ref. counted.

		   It is possible to reuse the same connection for several requests.

		   @return URLConnectionHndl a handle to the URLConnection
		*/
		URLConnectionHndl getConnection( void );
	private:
		string m_sFragment, m_sScheme, m_sAuth, m_sHost, m_sPath;
		int m_nPort;
		URLConnectionHndl m_scheme;
	};
	
	/**
	   Exception used if any syntactic errors occors.
	 */
	class URLException : public Exception {
	public:
		URLException( const string &sMsg );
		~URLException() throw();

		string getName( void ) const throw() {return "nbpp::URLException";}
	};

	typedef vector<pair<string, string> > options_t;

	/**
	   This is a base class for all URL connections, that defines the interface
	   that all subschemes need to use, to make communication possible.

	   By using this class together with the URLSchemes class it will be possible
	   to register several new connection types at runtime, to expand the possiblilityes 
	   of the URL class.

	   @see URLShemes
	 */
	class URLConnection : public RefCounted {
	protected:
		typedef vector<pair<string, string> > options_t;
		options_t m_in_header;
		URL m_url;
	public:
		/**
		   At least this instance need to know its related URL.

		   It keep its own copy, so the parrent URL will be able to change
		   without any problem for this connection instance.

		   @param url the url to communicate through
		 */
		URLConnection( const URL &url ) : m_url( url ) {}

		// Destructor
		virtual ~URLConnection() {}

		/**
		   Set the timeout for the current protocol, if a timeout have been reached
		   it will throw a TimeoutException.

		   @param nUsec number of milliseconds
		 */
		virtual void setTimeout( unsigned nUsec ) = 0;

		/**
		   Check to see if a serten option is set, on this scheme instance

		   @param sName the name of the option the check
		*/
		bool hasA( const string &sName ) const;

		/**
		   Get the value of the input header option of a serten name.

		   These values are first set after a successfull call to the perform
		   function, and contain any options set by the server side ... if any.

		   @param sName the name of the option value to retrive
		 */
		string operator[]( const string &sName ) const;

		/**
		   Set a header output value for for the next performe call. All values
		   will be send to the server (if able to handle), and is reset, when calling 
		   the perform function.

		   It is not possible to see the headers set by this function.

		   @param sName The name of the parameter
		   @param sValue the string value of the option
		 */
		virtual void set( const string &sName, const string &sValue ) = 0;

		/**
		   The scheme itself will not communicate to anyone, before this member
		   function have been called. When this member is called it will 
		   send the related request as the URL specify and additional options
		   that is either set on the related URL class.
		   
		   If the request when ok, all option values will be reset, and any new
		   ones found upon the resource reply will be added, to make it
		   possible to browse these after a successfull request.

		   If the request result in a data stream, this stream will be provided 
		   by the output stream.

		   Use this for GET and POST, depending on the the precense of the post
		   data.

		   @exception Throws an exception if any error occre.

		   @see getOutputStream, FormStorage

		   @param sPostData if a post need to be send, aply relevant data
		 */
		virtual void perform( const string &sPostData = "" ) = 0;

		/**
		   Use this for PUT to make it possible to pu large amount of
		   data.

		   If the user need to upload data to the requested service she will need 
		   to provide an istream from where data will be retrived. This stream can
		   of cause be NULL, and will then be ignored and no data will be uploaded.

		   @see getOutputStream

		   @param inputStream a ref to an input stream, but user must still 
		                       maintain the liftspan of this object.
		   @param nLength the number of bytes to send from stream

		 */
		virtual void perform( istream &inputStream, size_t nLength ) = 0;

		/**
		   If a request when OK, it may be possible to retrive incomming data 
		   by using this output stream.

		   The stream will be destroyed whenever this class is destroyed or then
		   a new call for perform have been done.
		 */
		virtual ostream &getOutputStream( void ) = 0;

		/**
		   If the request result in wery large amounts of data, it may be nessesary
		   to put it all directly into a output stream, and not using the internal
		   buffered version.

		   When calling perform, and data is returned from the server, it will end up
		   in this stream, and not the internel stream, normaly used. 

		   The stream must be valid until the perform function have been called, or 
		   until this class is destroyed. Caller need to keep the stream alive.

		   @param os the stream to put data into, if any
		 */
		virtual void setOutputStream( ostream &os ) = 0;
	};

	// Don't use this directly, internal class !
	class URLConnectionMakerBase : public RefCounted {
	public:
		virtual URLConnection *makeConnection( const URL &url ) const = 0;
	};

	// Internal template class
	template <class Connection> class URLConnectionMaker : public URLConnectionMakerBase {
	public:
		Connection *makeConnection( const URL &url ) const {
			return new Connection( url );
		}
	};

	/**
	   A singleton where to register new connection schemes for the URL class. If 
	   the scheme name allready exsist, it will be overwritten by the new inserted scheme,
	   and the old class will be freed.

	   All protocols need to register onto this, before using the getURLConnection on the 
	   URL class.

	   This will normaly be used via the URL class !
	 */
	class URLSchemes {
		typedef map<string, RefHandle<URLConnectionMakerBase> > connections_t;
		connections_t m_connections;
	public:
		/**
		   Get an instance of .... me :-)
		 */
		static URLSchemes &getInstance();

		/**
		   Do we have a scheme of this name ?
		 */
		bool hasA( const string &sScheme ) const;

		/**
		   Ok, make me a scheme class of this name on this URL, and yes
		   called need to take over ownership, by putting this inside a 
		   
		 */
		URLConnectionHndl operator[]( const URL &url ); 
		

		/**
		   Add a new URLConnectionMaker that will make sure to construct a connection
		   for a given URL scheme.
		 */
		template <class Connection> void add( const string &sName, 
											  URLConnectionMaker<Connection> *pMake ) {
			m_connections[ sName ] = pMake;
		}
	};
	
	// Utils
	string string_tolower( const string &str );
};

#endif /* _URL_HPP*/
