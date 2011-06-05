#include <osdep.hpp>
#include <iostream>
#include <unistd.h>

#include <vector>

#include <Thread.hpp>
#include <RWLocker.hpp>
#include <ExceptionGuard.hpp>

/*
 * Tests RWLocker.  A resource (consisting of a string) can be
 * read or written.  While it is being written, it is in an invalid
 * state.  Read and write locks are used to allow several reader
 * threads to read the resource repeatedly at times when it's safe to
 * do so.  NUM_WRITERS writer threads repeatedly get an exclusive lock
 * on the resource and updates it.  The read and write operations call
 * usleep() in order to hold onto the locks for a while.  In verbose mode,
 * (-v), the program outputs an 'r' for each read, 'R' for each failed read,
 * and a 'w' for each write.  If all the reads are successful, that's the only
 * output; otherwise the readers print the number of reads that failed.
 */

#define NUM_READERS 100
#define NUM_WRITERS 10 

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
    Message() : text("The Queen of England") { }

    string test() throw(AssertException, exception)
    {
        ReadLock readLock(locker);
        // Look busy for a while.
        reliable_usleep(1000 + random() / 100000);
        return text.c_str();
    }

    void update() throw(AssertException, exception)
    {
        WriteLock writeLock(locker);
        text = "message under construction";
        // Look busy for a while.
        reliable_usleep(2000 + random() / 10000);
        text = "The Queen of England";
    }

private:
    string text;
    RWLocker locker;
};


/*
 * Repeatedly calls read() on a Message.
 */
class ReaderImpl : public Runnable, public RefCounted
{
public:
    ReaderImpl(int arg_readerID, Message& arg_message, bool arg_testing)
        throw(AssertException, exception) :
        readerID(arg_readerID), message(arg_message), testing (arg_testing)
    {
    }

    ~ReaderImpl() throw() { }

    void run() throw()
    {
        try
        {
            int numOK = 0;
            int i;
            for (i = 1; i <= NUM_READS; i++)
            {
                if (message.test() == "The Queen of England")
                {
                    ++numOK;
                    if (!testing)
                    {
                        write (1, "r", 1);
                    }
                }
                else if (!testing)
                {
                    write (1, "R", 1);
                }
                
                // Delay a little bit...
                reliable_usleep (100 + random() / 1000);
            }
            
            if (numOK != NUM_READS)
            {
                cout << "Reader " << readerID << ": " <<
                    NUM_READS - numOK << "/" << NUM_READS << " reads failed" << endl;
            }
        }
        catch (Exception& e)
        {
            cerr << e.toString() << endl;
            return;
        }
        catch (exception& e)
        {
            cerr << e.what() << endl;
            return;
        }
    }

private:
    int readerID;
    Message& message;
    bool testing;
};
typedef RefHandle<ReaderImpl> Reader;


/*
 * Repeatedly calls update() on a Message.
 */
class WriterImpl : public Runnable, public RefCounted
{
public:
    WriterImpl(Message& arg_message, bool arg_testing) throw(AssertException, exception) :
        message(arg_message), testing(arg_testing) { }

    void run() throw()
    {
        try
        {
            for (int i = 1; i <= NUM_WRITES; i++)
            {
                message.update();
                if (!testing)
                    write (1, "w", 1);
                
                // Delay a little bit...
                reliable_usleep (100 + random() / 1000);
            }
        }
        catch (Exception& e)
        {
            cerr << e.toString() << endl;
            return;
        }
        catch (exception& e)
        {
            cerr << e.what() << endl;
            return;
        }
    }

    void cleanUp() throw() { }

private:
    Message& message;
    bool testing;
};

typedef RefHandle<WriterImpl> Writer;


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
        vector<Reader> readers;
        vector<Writer> writers;
        vector<Thread> threads;

        for (int i = 0; i < NUM_READERS; i++)
        {
            readers.push_back(Reader(new ReaderImpl(i, message, testing)));
            threads.push_back(Thread(*readers.back()));
        }

        for (int i = 0; i < NUM_WRITERS; i++)
        {
            writers.push_back(Writer(new WriterImpl(message, testing)));
            threads.push_back(Thread(*writers.back()));
        }

        for (vector<Thread>::iterator i = threads.begin(); i != threads.end (); i++)
            (*i).start();
        for (vector<Thread>::iterator i = threads.begin(); i != threads.end (); i++)
            (*i).join();

        if (!testing) cout << endl;
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
