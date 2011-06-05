#include <osdep.hpp>
#include <iostream>
#include <cstdlib>
#include <Socket.hpp>
#include <InetAddress.hpp>
#include <Assert.hpp>
#include <ExceptionGuard.hpp>

extern "C"
{
#include <signal.h>
#include <unistd.h>
}

/*
 * Uses a Socket to establish an HTTP connection to the host and
 * port of your choice.  Gets ten lines, then closes the connection.
 */

using namespace std;
using namespace nbpp;

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    bool OK = true;
    bool verbose = false;
    bool quiet = false;

    try
    {
        string host;
        int port;
        string file = "/";
        string expect;

        if (argc >= 3)
        {
            host = argv[1];
            port = atoi(argv[2]);
            if ((argc >= 4) && (string(argv[3]) == "-q"))
            {
                quiet = true;
            }
            else if (argc == 4)
            {
                file += argv[3];
            }
            verbose = true;
        }
        else
        {
            verbose = false;
            host = "localhost";
            port = 9876;
            expect = "hello";
        }

        // If a socket is unexpectedly closed and we keep sending output,
        // Linux will send SIGPIPE.  Socket::waitForInput() and
        // Socket::waitToSend() will notice that the socket is closed and
        // throw an exception, but we have to ignore SIGPIPE in case one
        // of these methods doesn't get a chance to check the socket for
        // errors before the signal is sent.
        signal(SIGPIPE, SIG_IGN);

        InetAddress addr = InetAddress::getByName(host, port);

        if (verbose && !quiet)
        {
            cout << "Connecting to " << addr.getAddress() << "..." << endl;
        }

        Socket sock(addr, 10); // 10 second timeout

        istream& is = sock.getInputStream();
 	ostream &os = sock.getOutputStream();	

        if (!quiet)
        {
            sock.waitToSend();
            os << "GET " << file << " HTTP/1.0" << endl << endl;
        }

        for (int i = 0; i < 10; i++)
        {
            sock.waitForInput();
            string buf;
            getline(is, buf);

            if (!expect.empty() && buf != expect)
            {
                OK = false;
            }
            
            if (verbose)
            {
                cout << buf << endl;
            }
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
            cout << 'n';
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
            cout << 'n';
        }

        return 1;
    }

    if (OK && !quiet)
    {
        cout << 'y';
    }

    return 0;
}
