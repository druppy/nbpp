//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

/**
 * \file Assert.hpp
 *
 * Contains the DEBUG macro and the Assert() function.
 */


#ifndef _NBPP_ASSERT
#define _NBPP_ASSERT

#include <nb++/osdep.hpp>

#include <string>

#ifndef DEBUG
#ifdef NDEBUG
#define DEBUG(x)        /* Debug assertion */
#else
#define DEBUG(x)        x
#endif
#endif

/**
 * \def DEBUG(x)
 *
 * Removes x from the source code if NDEBUG is defined.
 */

/**
 * All nb++ classes and functions are in the namespace <tt>nbpp</tt>.
 */
namespace nbpp
{
	/**
	 * Throws an object as an exception if an assertion is not met.  Can be used with the
	 * DEBUG macro to selectively remove assertions from production code.  For example:
	 *
	 * <pre>
	 * Assert(foo == bar, AssertException("failed!"));
	 * 
	 * DEBUG(Assert(foo == bar, AssertException("failed!")));
	 * </pre>
	 *
	 * The second assertion will be removed if NDEBUG is defined.
	 */
	template<class E> inline void Assert(bool assertion, E e)
    {
		if (!assertion) throw e;
	}
}

#endif /* _NBPP_ASSERT */
