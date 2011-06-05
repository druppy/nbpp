/**
   A nb++ url command line utility, like a simple curl, that also
   uses libcurl.

   This utility shows how simple and easy the nb++ URL classes are
   to impliment, and shows how to use these in a complete project.
*/
#include <Url.hpp>

#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>

using namespace nbpp;

int main( int argc, char *argv[] )
{
	if( argc < 2 ) {
		cerr << "Usage: nburl url [port_file]" << endl;
		return 2;
	}
	
	try {
		URL url( argv[ 1 ] );

		cout << "Url  : " << url.toString() << endl;
		cout << "Scheme " << url.getSchemeType() << endl;
		cout << "Host : " << url.getHost() << endl;
		
		cout << "Port : " << url.getPort() << endl;
		cout << "Path : " << url.getPath() << endl;

		URLConnectionHndl conn = url.getConnection();

		conn->setOutputStream( cout );
		if( argc > 2 ) {
			struct stat st;
			if( 0 == stat( argv[ 2 ], &st )) {
				char szData[ 2048 ];
				ifstream is( argv[ 2 ] );
				
				if( st.st_size >= sizeof( szData )) 
					conn->perform( is, st.st_size );  // PUT 
				else {
					is.read( szData, st.st_size );   // POST
					conn->perform( szData );
				}
			}
		}
		else
			conn->perform();
	} catch( const exception &ex ) {
		cerr << ex.what() << endl;
		return 1;
	}
	return 0;
}
