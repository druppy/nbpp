/**
   The body of out HTTP server !
*/
#include <ExceptionGuard.hpp>
#include <nb++/MultiThreader.hpp>

#include "HTTPServer.hpp"

using namespace nbpp;

int main( int argc, char *argv[] )
{
	ExceptionGuard exceptionGuard;

	try {
		if( argc > 1 ) {
			URL url( argv[ 1 ] );
			cout << "URL: " << url.toString() << endl;
		}
		else {
		HTTPServer svr( argv[ 0 ] );

		HTTPFileHandler *pFhndl = new HTTPFileHandler( "/files", "/tmp" );
		pFhndl->addMime( "html", "text/html" );
                pFhndl->addMime( "php", "text/plain" );
		pFhndl->addMime( "png", "image/png" );
                pFhndl->addMime( "gif", "image/gif" );

		HTTPFilter flt;

		flt.addPath( "/files" );
		flt.addExt( "html" );
		
		svr.addHandler( flt, pFhndl );

                svr.setThreader( MultiThreader() );
		cout << "Server is running ..." << endl;
		svr.run();  // Not being a daemon yet !
		}
	} catch( const Exception &ex ) {
		cerr << ex.toString() << endl;
	} catch( const exception &ex ) {
		cerr << ex.what() << endl;
	}
	cout << "Done" << endl;
}
