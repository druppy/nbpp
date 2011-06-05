#include <osdep.hpp>
#include <iostream>
#include <cstdlib>
#include <list>

extern "C"
{
#include <signal.h>
#include <unistd.h>
}

#include <RefCount.hpp>
#include <Thread.hpp>
#include <Mutex.hpp>
#include <ServerSocket.hpp>
#include <InetAddress.hpp>
#include <ExceptionGuard.hpp>

#if 1 /* old API */

/*
 * Uses ServerSocket to accept ten connections on the port of your choice.  Gets two lines
 * of text from each client, slowly sends 10 lines, then closes the connection.  Each
 * connection is handled in its own thread.
 */

#endif /* old API */

#if 0 /* new API */

/*
 * Uses ServerSocket to accept ten connections on the port of your choice.  Reads the
 * request from each client, sends the word "hello", waits two seconds, then closes the
 * connection.  Each connection is handled in its own thread.
 */

#endif /* new API */

using namespace std;
using namespace nbpp;

/*
 * Handles a connection in a new thread.
 */
class SimpleServerThread : public Runnable, public RefCounted
{
public:
    SimpleServerThread(Socket arg_clientSock) :
        clientSock(arg_clientSock), done(false) { }

    virtual ~SimpleServerThread() throw() { }

    void start() throw(AssertException, exception)
    {
        thread = Thread(*this);
        thread.start();
    }

    void run() throw()
    {

#if 1 /* old API */

        iostream& sockStream = clientSock.getIOStream();

        try
        {
            for (int i = 0; i < 2; i++)
            {
                clientSock.waitForInput();
                string buf;
                getline(sockStream, buf);
            }

            for (int i = 0; i < 10; i++)
            {
                clientSock.waitToSend();
                sockStream << "hello" << endl;
                reliable_usleep(200000);
            }

#endif /* old API */

#if 0 /* new API */

        ostream& sockStream = clientSock.getOutputStream();

        try
        {
	    string buf;
	    clientSock.waitForInput();
            clientSock.read(buf);

	    clientSock.waitToSend();
	    sockStream << "hello" << endl;
	    reliable_usleep(200000);

#endif /* new API */

        }
        catch (Exception& e)
        {
            cout << e.toString() << endl;
        }

        clientSock.close();

        Lock lock(mutex);
        done = true;
    }

    void join() throw(ThreadControlException, AssertException, exception)
    {
        thread.join();
    }

    bool isDone() throw(AssertException, exception)
    {
        Lock lock(mutex);
        return done;
    }
        
private:
    Socket clientSock;
    bool done;
    Thread thread;
    Mutex mutex;
};

typedef RefHandle<SimpleServerThread> ThreadHandle;

/*
 * A predicate that returns true if a SimpleServerThread is done.
 */
class IsDone
{
public:
    bool operator()(ThreadHandle thread) const
        throw(ThreadControlException, AssertException, exception)
    {
        if (!thread->isDone())
            return false;

        thread->join();
        return true;
    }
};

/*
 * Accepts connections and dispatches them to SimpleServerThread
 * objects.
 */
int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    bool verbose = false;

    try
    {
        int port;
        bool verbose;

        if (argc == 2)
        {
            port = atoi(argv[1]);
            verbose = true;
        }
        else
        {
            port = 9876;
            verbose = false;
        }

        // If a socket is unexpectedly closed and we keep sending output,
        // Linux will send SIGPIPE.  Socket::waitForInput() and
        // Socket::waitToSend() will notice that the socket is closed and
        // throw an exception, but we have to ignore SIGPIPE in case one
        // of these methods doesn't get a chance to check the socket for
        // errors before the signal is sent.
        signal(SIGPIPE, SIG_IGN);

        list<ThreadHandle> threads;

        InetAddress addr = InetAddress::getAnyLocalHost(port);

        if (verbose)
        {
            cout << "Creating socket on " << addr.getName() << ":" << port << endl;
        }

        ServerSocket sock(addr, 20);

        for (int i = 0; i < 10; i++)
        {
            if (verbose)
            {
                cout << "Waiting for a connection..." << endl;
            }

            InetAddress clientAddr = InetAddress::allocEmptyAddress();
            Socket clientSock = sock.accept(clientAddr);
            clientSock.setTimeout(20);
                        
            if (verbose)
            {
                cout << "Got connection from " << clientAddr.getAddress()
                     << ", port " << clientAddr.getPort() << endl;
            }

            ThreadHandle thread(new SimpleServerThread(clientSock));
            threads.push_back(thread);
            thread->start();

            // Deallocate any threads that have finished.
            threads.remove_if(IsDone());
        }

        // Wait for the remaining threads to finish.
        if (verbose)
        {
            cout << "Waiting for remaining threads to finish..." << endl;
        }

        for (list<ThreadHandle>::iterator iter = threads.begin();
             iter != threads.end();
             ++iter)
        {
            ThreadHandle thread = *iter;
            thread->join();
        }
    }
    catch (Exception& e)
    {
        if (verbose)
        {
            cout << e.toString() << endl;
        }
        else
        {
            cout << 'N';
        }

        return 1;
    }
    catch (exception& e)
    {
        if (verbose)
        {
            cerr << e.what() << endl;
        }
        else
        {
            cout << 'N';
        }

        return 1;
    }

    sleep(1);

    if (verbose)
    {
        cout << "Done." << endl;
    }
    else
    {
        cout << 'Y';
    }

    return 0;
}
