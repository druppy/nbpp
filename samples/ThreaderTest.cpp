/**
   Test the differant threaders, and there ability to handle a big load without 
   resource load and stalling.
*/
// #define _PER_COMMAND

#include <Command.hpp>
#include <Thread.hpp>
#include <ThreadPerCommand.hpp>
#include <MultiThreader.hpp>
#include <ExceptionGuard.hpp>

#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace nbpp;
using namespace std;

int g_nCommandCount = 0;
int g_nVoid;

string g_sPostfix = " bar";
string g_sItem = "X";

class Worker : public CommandImpl {
	const string m_sPrefix;
	vector<string> m_strs;
public:
	Worker( const string &sPrefix, vector<string> &strs ) : m_sPrefix( sPrefix ), m_strs( strs ) {
		// cout << __PRETTY_FUNCTION__ << endl;

		g_nCommandCount++;
	}

	virtual ~Worker() throw() {
		// cout << __PRETTY_FUNCTION__ << endl;

		g_nCommandCount--;
	}

	void execute( void );
};

void Worker::execute( void )
{
	string sTmp = m_sPrefix;
	int cnt = 0;
	int max = rand();

	// Thread::sleep( rand() % 10 );

	while( cnt++ < 10000 ) {
		//for( int i = 0; i != max; i++ );
		string str;

		if( cnt % 16 == 0 )
			sTmp += g_sItem;

		vector<string>::const_iterator iter;
	 	for( iter = m_strs.begin(); iter != m_strs.end(); iter++ )
			; //str += *iter;
	}
	
	sTmp += g_sPostfix;

	g_nVoid = cnt;

	cout << "<";
}

int main( int argc, char *argv[] )
{
	string sPrefix = "Foo ";
	ExceptionGuard exceptionGuard;

	try {
#ifdef _PER_COMMAND
		ThreadPerCommand threader;
#else
		MultiThreader threader;
		threader.getThreadPool().setMaxThreads( 10 );
#endif

		vector<string> strs;

		for( int i = 0; i != 1000; i++ ) {
			ostringstream os;

			os << i;

			strs.push_back( os.str());
		} 

		while( true ) {
			while( g_nCommandCount > 100 ) {
				cout << "S" << endl;
				Thread::sleep( 1 );
			}

			Command cmd( new Worker( sPrefix, strs ));
			
			threader.queue( cmd );
		
			cout << ">";
			cout.flush();
		}
	} catch( const exception &ex ) {
		cerr << "Exception : " << ex.what() << endl;
	}
	cout.flush();
	cout << "Cleaning up .. " << endl;
	while( g_nCommandCount ) {
		cout << "Remaining threads " << g_nCommandCount << endl;
		Thread::sleep( 5 );
	}
}
