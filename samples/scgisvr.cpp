/**
   The body of out HTTP server !
*/
#include <ExceptionGuard.hpp>
#include <nb++/MultiThreader.hpp>

#include <nb++/SCGIServer.hpp>

using namespace nbpp;

class hello_handler : public HTTPRequestHandler
{
public:
    Result handle( Request &req ) {
        req.getOutputStream() << "Hello, world";
        
        cout << "hello world request added" << endl;
        
        return HTTP_OK;
    }
};

int main( int argc, char *argv[] )
{
	ExceptionGuard exceptionGuard;

	try {
		SCGIServer svr( "test_scgi" );

		HTTPFilter flt; // Not used really !

        flt.addPath( "/scgi" );
        
		svr.addHandler( flt, new hello_handler() );

        svr.setThreader( MultiThreader() );
		cout << "SCGI Server is running ..." << endl;
		svr.run();  // Not being a daemon yet !
	} catch( const Exception &ex ) {
		cerr << ex.toString() << endl;
	} catch( const exception &ex ) {
		cerr << ex.what() << endl;
	}
	cout << "Done" << endl;
}
