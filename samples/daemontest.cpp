#include <NetworkDaemon.hpp>
#include <ForkThreader.hpp>
#include <MultiThreader.hpp>
#include <ThreadPerCommand.hpp>
#include <InetAddress.hpp>
#include <ExceptionGuard.hpp>

/*
 * Demonstrates Daemon class.
 *
 * When you connect to port 8080, the daemon answers with a 'Hello world!' message,
 * pauses 20 seconds, and closes the connection.  If you send a SIGHUP to the daemon,
 * 'Reloaded ' is prepended to the text.  The daemon exits on SIGTERM.
 *
 * The default threading model is SingleThreadingImpl, which handles all requests in a
 * single process, without multithreading.
 *
 * You can change the threading model by calling the daemon with:
 *
 * -f:
 *      * Daemon forks for each incoming request.
 * -t:
 *      * Daemon spawns a new thread for each incoming request.
 * -p:
 *      * Daemon maintains a pool of threads, that grow/shink depending on load.
 *
 */


using namespace std;
using namespace nbpp;

class TestDaemon : public NetworkDaemon<InetAddress>
{
public:
    TestDaemon (const string& arg_programName) throw(AssertException, exception) :
        NetworkDaemon<InetAddress>(arg_programName), text("Hello world!"), quiet(false)
    { }
    
    void setQuiet()
    { quiet = true; }

    ~TestDaemon() throw() { }
protected:
    virtual void handleConnection(NetworkConnection<InetAddress>& connection)
        throw(Exception, exception);
    virtual bool handleException(Exception& e) throw(exception);
    virtual void reload() throw(Exception, exception);
    virtual ServerSocket initServerSocket() throw(Exception, exception);

private:
    string text;
    bool quiet;
};

void TestDaemon::handleConnection(NetworkConnection<InetAddress>& connection)
    throw(Exception, exception)
{
    if (quiet)
    {
        for (int i = 0; i < 10; i++)
	{
            connection.sock.getOutputStream () << text << endl;
	}
    }
    else
    {
        connection.sock.getOutputStream () << "[" << ::getpid () << "]: "
                                           << text << endl;
        reliable_usleep(20000);
    }

    connection.sock.getOutputStream ().flush();
    connection.sock.close();
}

bool TestDaemon::handleException(Exception& e) throw(exception)
{
    return true;
}

void TestDaemon::reload() throw(Exception, exception)
{
    text = "Reloaded " + text;
}

ServerSocket TestDaemon::initServerSocket() throw(Exception, exception)
{
    return ServerSocket(InetAddress::getAnyLocalHost(8080));
}


int main (int argc, char** argv)
{
    ExceptionGuard exceptionGuard;

    try
    {
        TestDaemon daemon (argv [0]);

        if ((argc > 1) && (*argv [1] == '-'))
        {
            switch (argv [1][1])
            {
            case 'f':
                daemon.setThreader(ForkThreader());
                break;
            case 't':
                daemon.setThreader(ThreadPerCommand());
                break;
            case 'p':
                daemon.setThreader(MultiThreader());
                break;
            case 'q':
                daemon.setQuiet();
                break;
            default:
                cerr << "Usage: " << argv[0] << " [{-f|-t|-p}]" << endl;
                return 1;
            }
        }
        pid_t pid = daemon.detach();
        if (pid == 0)
        {
            return daemon.run();
        }
        else
        {
            cout << pid << endl;
        }
    }
    catch (Exception& e)
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
