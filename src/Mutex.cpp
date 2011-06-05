//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/Assert.hpp>
#include <nb++/Exception.hpp>
#include <nb++/Mutex.hpp>

#include <assert.h>

#include <list>
#include <algorithm>

namespace nbpp
{
	using namespace std;
	/*
	  This is an internal class that maintail a list of all allocated mutexes
	  to keep track of them, if a fork or popen function will be called to 
	  make it possible to lock and unlock all at ones, for the fock process to
	  succed.

	  Im still not sure this is the way to do this, and I really like to eliminate 
	  the use of fork anyware in nb++, but just in case :-)
	 */
	class MutexManager {
		typedef list<pthread_mutex_t *> mutexes_t;
		mutexes_t m_mutexes;
        pthread_mutex_t m_mutex;
        pthread_mutexattr_t m_mutexAttr;

		static MutexManager *m_pManager;
		MutexManager() {
			reg_funcs();
			pthread_mutexattr_init( &m_mutexAttr );
			pthread_mutex_init( &m_mutex, &m_mutexAttr );
		}
		
		~MutexManager() {
			pthread_mutex_destroy( &m_mutex );
			pthread_mutexattr_destroy( &m_mutexAttr );
		}

		void reg_funcs( void );
	public:
		static MutexManager &getInst( void ) {
			if( !m_pManager )
				m_pManager = new MutexManager();

			return *m_pManager;
		}

		void add( pthread_mutex_t &mutex ) {
			pthread_mutex_lock( &m_mutex );

			m_mutexes.push_back( &mutex );

			pthread_mutex_unlock( &m_mutex );
		}

		void remove( pthread_mutex_t &mutex ) {
			pthread_mutex_lock( &m_mutex );
			mutexes_t::iterator i = find( m_mutexes.begin(), m_mutexes.end(), &mutex );

			if( i != m_mutexes.end())
				m_mutexes.erase( i );

			pthread_mutex_unlock( &m_mutex );
		}
		
		void initAll( void ) {
			pthread_mutex_lock( &m_mutex );

			// cout << "Init all " << m_mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = m_mutexes.begin(); i != m_mutexes.end(); i++ )
				::pthread_mutex_init( *i, NULL );

			pthread_mutex_unlock( &m_mutex );
		}

		void lockAll( void ) {
			pthread_mutex_lock( &m_mutex );
			// cout << "Lock all " << m_mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = m_mutexes.begin(); i != m_mutexes.end(); i++ )
				::pthread_mutex_lock( *i );

			pthread_mutex_unlock( &m_mutex );
		}

		void releaseAll( void ) {
			pthread_mutex_lock( &m_mutex );
			// cout << "Release all " << m_mutexes.size() << endl;

			mutexes_t::iterator i;
			for( i = m_mutexes.begin(); i != m_mutexes.end(); i++ )
				::pthread_mutex_unlock( *i );

			pthread_mutex_unlock( &m_mutex );
		}
	};

	void mutex_init_prepare( void ) {
	}

	void mutex_init_parent( void ) {
	}

	void mutex_init_child( void ) {
		MutexManager::getInst().initAll();
	}

	MutexManager *MutexManager::m_pManager = NULL;

	void MutexManager::reg_funcs( void ) 
	{
		int res = ::pthread_atfork( mutex_init_prepare, mutex_init_parent, mutex_init_child );

		assert( res == 0 );
	}

	////////////////////
	// Impl. of Mutex

    Mutex::Mutex() throw(AssertException, exception)
    {
        pthread_mutexattr_init( &m_mutexAttr );
        pthread_mutex_init( &m_mutex, &m_mutexAttr );
        pthread_condattr_init( &m_condAttr ); 
        pthread_cond_init( &m_cond, &m_condAttr );
		
		// MutexManager::getInst().add( m_mutex );
    }

    Mutex::~Mutex() throw()
    {
		// MutexManager::getInst().remove( m_mutex );

		pthread_mutexattr_destroy( &m_mutexAttr );
        pthread_mutex_destroy( &m_mutex );
		pthread_condattr_destroy( &m_condAttr );		
        pthread_cond_destroy( &m_cond );
    }

    void Mutex::lock() throw(AssertException, exception)
    {
        pthread_mutex_lock( &m_mutex );
    }

    void Mutex::unlock() throw(AssertException, exception)
    {
        pthread_mutex_unlock( &m_mutex );
    }

    void Mutex::wait() throw(AssertException, exception)
    {
        pthread_cond_wait( &m_cond, &m_mutex );
    }

    void Mutex::wait(unsigned int sec) throw(AssertException, exception)
    {
        timespec now;
        clock_gettime(CLOCK_REALTIME, &now);
        now.tv_sec += sec;
        pthread_cond_timedwait(&m_cond, &m_mutex, &now);
    }

    void Mutex::notify() throw(AssertException, exception)
    {
        pthread_cond_signal( &m_cond );
    }

    void Mutex::notifyAll() throw(AssertException, exception)
    {
        pthread_cond_broadcast( &m_cond );
    }

	/////////////////////////////
	// Impl. of RecursiveMutex

    RecursiveMutex::RecursiveMutex() throw(AssertException, exception) :
        m_lockCount(0)
    { }

    RecursiveMutex::~RecursiveMutex() throw() { }

    void RecursiveMutex::lock() throw(AssertException, exception)
    {
        if (!m_lockCount || !pthread_equal(m_owner, pthread_self())) {
            Mutex::lock();
            m_owner = pthread_self();
        }
        ++m_lockCount;
    }

    void RecursiveMutex::unlock() throw(AssertException, exception)
    {
        DEBUG(Assert(m_lockCount > 0,
                     AssertException("nbpp::RecursiveMutex::unlock(): "
                                     "mutex already unlocked")));
        DEBUG(Assert(pthread_equal(m_owner, pthread_self()),
                     AssertException("nbpp::RecursiveMutex::unlock(): "
                                     "mutex locked by another thread")));
        if (--m_lockCount == 0)
            Mutex::unlock();
    }

    void RecursiveMutex::wait() throw(AssertException, exception)
    {
        unsigned int saveCount = m_lockCount;
        m_lockCount = 0;
        Mutex::wait();
        m_owner = pthread_self();
        m_lockCount = saveCount;
    }

    void RecursiveMutex::wait(unsigned int sec) throw(AssertException, exception)
    {
        unsigned int saveCount = m_lockCount;
        m_lockCount = 0;
        Mutex::wait(sec);
        m_owner = pthread_self();
        m_lockCount = saveCount;
    }

    Lock::Lock(Mutex& arg_mutex, bool autoLock) throw(AssertException, exception) :
        m_mutex(arg_mutex), m_locked(false)
    {
        if ( autoLock ) 
			get();
    }

    Lock::~Lock() throw()
    {
        try {
            release();
        } catch (...) { }
    }

    void Lock::get() throw(AssertException, exception)
    {
        if ( !m_locked ) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void Lock::release() throw(AssertException, exception)
    {
        if ( m_locked ) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void Lock::wait() throw(AssertException, exception)
    {
        DEBUG(Assert(m_locked,
                     AssertException("nbpp::Lock::wait(): "
                                     "wait on unlocked mutex")));
        m_mutex.wait();
    }

    void Lock::wait(unsigned int sec) throw(AssertException, exception)
    {
        DEBUG(Assert(m_locked,
                     AssertException("nbpp::Lock::wait(sec): "
                                     "wait on unlocked mutex")));
        m_mutex.wait(sec);
    }


    void Lock::notify() throw(AssertException, exception)
    {
        DEBUG(Assert(m_locked,
                     AssertException ("nbpp::Lock::notify(): "
                                      "notify on unlocked mutex")));
        m_mutex.notify();
    }

    void Lock::notifyAll() throw(AssertException, exception)
    {
        DEBUG(Assert(m_locked,
                     AssertException("nbpp::Lock::notifyAll(): "
                                     "notifyAll on unlocked mutex")));
        m_mutex.notifyAll();
    }
}
