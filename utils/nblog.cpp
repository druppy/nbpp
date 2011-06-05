/**
   Small command line util, for logging while using the nb++ std.
   log format.

   The format of the nblog util is this :

   nblog [-f filename] [-c number] text

   where -f tells nblog where to append log output, and -c gives the log
   number. The text can be either plain text or value pairs in the format
   
   name=value;

   It is possible to define the logfile name in the environment variable :

   NBPP_LOGFILE

   and this will be overwriten by the -f parameter.

   return 0 if ok, 1 if missing parameters and 3 on error.
*/
#include <cstdlib>

#include <nb++/Log.hpp>
#include <nb++/CommandLineOptions.hpp>

using namespace nbpp;
using namespace std;

int main( int argc, char *argv[] )
{
	try {
		CommandLineOptions options( argc, argv );

		options.add( "Output log file",    'f', "file", "file", 
			CommandLineOptions::Required );
		options.add( "Log code",           'c', "code", "codeno",  
			CommandLineOptions::Required );
		options.add( "Help text",          'h', "help" );
	
		options.parse();

		if( options[ 'h' ] || argc == 1 ) {
			options.usage();
			return 1;
		}

		string sFileName;

		if( options[ 'f' ] ) 
			sFileName = options[ 'f' ].toString();
		else {
			char *pszFname = getenv( "NBPP_LOGFILE" );
			if( pszFname )
				sFileName = pszFname;
		}

		if( !sFileName.empty()) {
			RefHandle<FileLogger> flog( new FileLogger( sFileName ));
 
			nbpp::log.set( flog );
		}

		if( options[ 'c' ] )
			nbpp::log.setCode( options[ 'c' ].toInt());
		
		string sText;
		const OptionValues &values = options.getUnmarkedArgs();
		OptionValues::const_iterator i = values.begin();
		while( i != values.end()) {
			sText += i->toString();
			++i;
		}

		nbpp::log << sText << endl;

	} catch( const exception &ex ) {
		cerr << "Error : " << ex.what() << endl;
		return 2;
	}

	return 0;
}
