/**
   Log translate.

   Takes std. in and read it line by line, and output these merged with the
   propper log text.
*/
#include <nb++/Log.hpp>
#include <algorithm>

using namespace nbpp;

int main( int argc, char *argv[] )
{
	LogDefinition ldef;

	try {
		if( argc > 1 )
			ldef.loadDefs( argv[ 1 ] );

		const char *pszPath = getenv( "NBPP_TL_PATH" );
		if( pszPath ) {
			string sPath( pszPath );

			string::iterator i, i_start = sPath.begin();
			i = i_start;
			do {
				string sFile;
				
				i = find( i, sPath.end(), ':' );
				if( i == sPath.end()) {
					sFile = string( i_start, sPath.end());
					i_start = i;
				}
				else
					sFile = string( i_start, i++ );

				cout << "Info: Loading from def file : " << sFile << endl;
				ldef.loadDefs( sFile );
				i_start = i;
			} while( i_start != sPath.end());
		}
		else
			cout << "Warning: The env. \"NBPP_TL_PATH\" is missing." << endl;

		string sLine;
		
		while( getline( cin, sLine )) {
			LogLine l = ldef.parseLine( sLine );
			cout << l.getTimestamp().toString() << ": " << l.getText() << endl;
		}
	} catch( const exception &ex ) {
		cerr << "Error : " << ex.what() << endl;
		return 1;
	}
	return 0;
}
