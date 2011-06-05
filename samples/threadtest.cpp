#include <osdep.hpp>
#include <iostream>
#include <unistd.h>
#include <Thread.hpp>
#include <RWLocker.hpp>
#include <ExceptionGuard.hpp>

/*
 * Tests RWLocker.  A resource (consisting of a string) can be
 * read or written.  While it is being written, it is in an invalid
 * state.  Read and write locks are used to allow several reader
 * threads to read the resource repeatedly at times when it's safe to
 * do so.  One writer thread repeatedly gets an exclusive lock on the
 * resource and updates it.  The read and write operations call
 * usleep() in order to hold onto the locks for a while.  The program
 * outputs an 'r' for each read, and a 'w' for each write.  If all
 * the reads are successful, that's the only output; otherwise the
 * readers print the number of reads that failed.
 */

#define NUM_READS 100
#define NUM_WRITES 100

using namespace std;
using namespace nbpp;

/*
 * If the locks are working properly, the test() method should always
 * return "The Queen of England".
 */
class Message
{
public:
    Message() throw(exception) : text("The Queen of England") { }

    string test() throw(AssertException, exception)
    {
        ReadLock readLock(locker);
        // Look busy for a while.
        reliable_usleep(1000);
        return text.c_str();
    }

    void update() throw(AssertException, exception)
    {
        WriteLock writeLock(locker);
        text = "message under construction";
        // Look busy for a while.
        reliable_usleep(2000);
        text = "The Queen of England";
    }

private:
    string text;
    RWLocker locker;
};


/*
 * Repeatedly calls read() on a Message.
 */
class Reader : public Runnable
{
public:
    Reader(int arg_readerID, Message& arg_message, RWLocker& arg_go, bool arg_testing)
        throw(AssertException, exception) :
        readerID(arg_readerID), message(arg_message), go(arg_go), testing (arg_testing)
    {
    }
    
    ~Reader() throw() { }
    
    void run() throw()
    {
        try
        {
            ReadLock readLock(go);
            
            int numOK = 0;
            int i;
            for (i = 1; i <= NUM_READS; i++)
            {
                if (message.test() == "The Queen of England")
                {
                    ++numOK;
                }
                if (!testing)
                    cout << 'r' << flush;
                
                // Delay a little bit...
                reliable_usleep (100);
            }
            
            if (numOK != NUM_READS)
            {
                cout << "Reader " << readerID << ": " <<
                    NUM_READS - numOK << "/" << NUM_READS << " reads failed" << endl;
            }
            else if (testing)
            {
                cout << "Reader OK" << endl << flush;
            }
        }
        catch (const Exception& e)
        {
            cerr << e.toString() << endl;
            return;
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
            return;
        }
    }

private:
    int readerID;
    Message& message;
    RWLocker& go;
    bool testing;
};


/*
 * Repeatedly calls update() on a Message.
 */
class Writer : public Runnable
{
public:
    Writer(Message& arg_message, RWLocker& arg_go, bool arg_testing)
        throw(AssertException, exception) :
        message(arg_message), go(arg_go), testing(arg_testing) { }

    void run() throw()
    {
        try
        {
            ReadLock readLock(go);
            
            for (int i = 1; i <= NUM_WRITES; i++)
            {
                message.update();
                if (!testing)
                    cout << 'w' << flush;
                
                // Delay a little bit...
                reliable_usleep (100);
            }
        }
        catch (const Exception& e)
        {
            cerr << e.toString() << endl;
            return;
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
            return;
        }
    }

private:
    Message& message;
    RWLocker& go;
    bool testing;
};


/*
 * Creates a Writer and three Readers, and lets them do their thing
 * until they're done.
 */
int main(int argc, char* argv[])
{
    ExceptionGuard exceptionGuard;

    try
    {
        bool testing = argc < 2;

        Message message;
        RWLocker go;
        WriteLock wlock(go);

        Reader r1 = Reader(1, message, go, testing);
        Reader r2 = Reader(2, message, go, testing);
        Reader r3 = Reader(3, message, go, testing);
        Writer w = Writer(message, go, testing);

        Thread rt1(r1);
        Thread rt2(r2);
        Thread rt3(r3);
        Thread wt(w);

        rt1.start();
        rt2.start();
        rt3.start();
        wt.start();

        reliable_usleep(10000);
        wlock.release();

        rt1.join();
        rt2.join();
        rt3.join();
        wt.join();

        if (!testing) cout << endl;
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
