/**
 	Refcount handler template class.

	Why not use the original handler in the nb++ framework ?

	This is because it will be possible to save the same pointer in two differant 
	handlers while one handler does not know when the other have released the pointer
	and this could (and will) lead to an error.

	The ref counter follow the pointer and not the handler. 

	This leed to more simple code, and that only makes things more stabel :-)
 */
#ifndef __REFCOUNT_HPP__
#define __REFCOUNT_HPP__

#include <iostream>
#include <assert.h>

#include <nb++/Mutex.hpp>

namespace nbpp {
class BaseRefHandle;

/**
   All elements controlled by the RefHandler class must enherit from this class
   to enable the propper counter. There is no need to know anything about this 
   class other than it need to be part of the class you like to make referance 
   counting onto.

   @see RefHandle
*/
class RefCounted {
friend class BaseRefHandle;
protected:
	virtual ~RefCounted() = 0;

public:
	explicit RefCounted( void ) : m_nNbppRefCount( 0 ), m_bDestroying( false ) {}
	RefCounted( const RefCounted & ) : m_nNbppRefCount( 0 ), m_bDestroying( false ) {}

	/// Peek the ref count value, not really thread safe !
	int peekRefCount( void ) const {return m_nNbppRefCount;}

	// RefCounted &operator=( const RefCounted & ) {return *this;}
private:
	void lock( void ) {
		Lock sync( m_mutex );

		if( m_bDestroying )
			throw RefCountException( "Can't lock a destructed data object." ); 

		m_nNbppRefCount++;
	}

	void release( void ) {
		Lock sync( m_mutex );

		if( --m_nNbppRefCount == 0 ) {
			m_bDestroying = true;
			sync.release();
			delete this;    // Are we in danger here, or does'nt out refcounter help us out ?
		}
	}

	mutable int m_nNbppRefCount;
	Mutex m_mutex;
	mutable bool m_bDestroying;
};

	inline RefCounted::~RefCounted() {}

class BaseRefHandle {
public:
	BaseRefHandle( RefCounted *pRef ) : m_pRef( pRef ) {
		if( m_pRef ) {
			m_pRef->lock();
		}
	}

	virtual ~BaseRefHandle() {
		if( m_pRef  )
			m_pRef->release();
	}
	
	BaseRefHandle( const BaseRefHandle &refHndl ) {
		m_pRef = refHndl.m_pRef;

		if( m_pRef )
		  	m_pRef->lock();
	}

	BaseRefHandle &operator=( const BaseRefHandle &refHndl ) {
		if( m_pRef != refHndl.m_pRef ) {
			RefCounted *pOld = m_pRef;
			
			m_pRef = refHndl.m_pRef;

			if( m_pRef )
				m_pRef->lock();

			if( pOld )
				pOld->release();			
		}
		return *this;
	}
	
protected:
	RefCounted *m_pRef;
};

/**
   A referance handler class takeing a class that at least enhirits from the 
   RefCounted class, but else can be anything specified in the template parameter.

   If more that one handler have a reference to the same RefCounted data, they do
   not need to be aware of this fact, as the counter ellement is to be found on the
   RefCounted class.

   @see RefCounted
*/
template<class T> class RefHandle : protected BaseRefHandle {
 public:
	/**
	   Make a new handler, and if pRef is set make a reference to this class.

	   @param pRef
	 */
	RefHandle( T *pRef = NULL ) : BaseRefHandle( pRef ) {}

	/**
	   Make a new handle and a referende to the data pointed to by hndl.
	 */
	RefHandle( const RefHandle<T> &hndl ) : BaseRefHandle( hndl ) {}

	/**
	   Copy and refere to the data pointed to by hndl, and release data
	   holed onto by this handler.
	 */
	RefHandle<T> &operator=( const RefHandle<T> &hndl ) {
		*((BaseRefHandle *)this) = hndl;

		return *this;
	}
	
	/**
	   Converts a RefHandle<T>; into a RefHandle<T2>.  The conversion will compile if a T*
	   can be converted to a T2*, e.g. if T is a class derived from T2.
	 */
	template<class T2> operator RefHandle<T2>() const {
		return RefHandle<T2>( get() );	
	}		

	/**
	   Does the same as operator ->, as it get a pointer to the refered handler.
	 */
	T *get( void ) const throw() { return (T *)m_pRef;}

	/**
	   Return refered data as reference.
	 */
	T &operator *() const {return *((T *)m_pRef);}

	/**
	   Return refered data as a pointer.
	 */
	T *operator ->() const throw() {return (T *)m_pRef;}
	
	operator bool( void ) const {return m_pRef;}
};
}
#endif
