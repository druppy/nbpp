#include <InetAddress.hpp>
#include <ExceptionGuard.hpp>
#include <iostream>

using namespace std;
using namespace nbpp;

/**
 * Test InetAddress::getAllByName().  Given a host name, prints its canonical name and IP
 * addresses.
 */

int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        string host;
        bool verbose;
        
        if (argc == 2)
        {
            host = argv[1];
            verbose = true;
        }
        else
        {
            host = InetAddress::getAnyLocalHost().getName();
            verbose = false;
        }

        list<InetAddress> addrList = InetAddress::getAllByName(host);

        if (verbose)
        {
            cout << "Canonical name: " << addrList.begin()->getName() << endl;
            cout << "IP addresses:" << endl;
        }

        for (list<InetAddress>::iterator iter = addrList.begin();
             iter != addrList.end();
             ++iter)
        {
            cout << iter->getAddress() << endl;
        }
    }
    catch (const Exception& e)
    {
        cerr << e.toString() << endl;
        return 1;
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}
