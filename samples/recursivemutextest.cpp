#include <osdep.hpp>
#include <iostream>
#include <unistd.h>
#include <Thread.hpp>
#include <Mutex.hpp>
#include <ExceptionGuard.hpp>

/*
 * Tests RecursiveMutex.  Two threads recursively lock the mutex and
 * print their id every time their lock succeeds.  Output should be
 * a series of ten '1's and then ten '2's. (not intermixed)
 */

using namespace std;
using namespace nbpp;

/*
 * Recursively lock the mutex
 */
class MutexLocker : public Runnable
{
public:
    MutexLocker(Mutex& arg_mutex, int arg_id) : mutex(arg_mutex),id(arg_id) { }

    void do_lock(int i = 0)
    {
        Lock lock(mutex);
        cout << id << flush;
        reliable_usleep(10000);
        if (++i < 10)
            do_lock(i);
        else
            sleep(1);
    }
    void run() throw()
    {
        do_lock();
    }

private:
    Mutex& mutex;
    int id;
};


/*
 * Creates two MutexLockers, and lets them do their thing
 * until they're done.
 */
int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        RecursiveMutex mutex;
        MutexLocker ml1 = MutexLocker(mutex, 1);
        MutexLocker ml2 = MutexLocker(mutex, 2);

        Thread mlt1(ml1);
        Thread mlt2(ml2);

        mlt1.start();
        reliable_usleep(5000);
        mlt2.start();

        mlt1.join();
        mlt2.join();

        cout << endl;
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
