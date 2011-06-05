//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/ThreadPerCommandImpl.hpp>
#include <nb++/Thread.hpp>

namespace nbpp
{
	class CommandWorker : public Runnable {
	public:
		CommandWorker( Command command ) : m_command( command ) {}
		virtual ~CommandWorker() throw() {}

		void run( void ) throw() {
			m_command.execute();

			delete this;       // Ok, this is not nice, but this is what we need
		}

	private:
		Command m_command;
	};
	
	ThreadPerCommandImpl::ThreadPerCommandImpl()
    {
	}

	void ThreadPerCommandImpl::queue( Command command )
	{
		using namespace std;

		try {
			CommandWorker *pWorker = new CommandWorker( command );

			Thread thread( *pWorker, false );

			thread.start();
		} catch( const exception &ex ) {
			cerr << "Command exception " << ex.what() << endl;
		} catch( ... ) {
			cerr << "Unknown command exception" << endl;
		}
	}

	void ThreadPerCommandImpl::doInit() 
	{
	}
}
