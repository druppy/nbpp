//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/osdep.hpp>
#include <nb++/TempFileImpl.hpp>

#if __GNUG__ < 3
# 	include <strstream>
#   define ostringstream ostrstream
#else
# 	include <iostream>
# 	include <fstream>
#	include <sstream>
#endif

extern "C"
{
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <cstdlib>
}

namespace nbpp
{
    using std::ios;
    using std::ostringstream;
    using std::ends;
    using std::ofstream;
    using std::ifstream;

    TempFileImpl::TempFileImpl(const string& prefix, const string& suffix)
        throw(IOException, AssertException, exception)
    {
        string dir;

        // Try TMPDIR.
        char* envDir = getenv("TMPDIR");
        if (!(envDir == NULL || strlen(envDir) == 0))
        {
            dir = envDir;
            if (makeFile(dir, prefix, suffix))
            {
                return;
            }
        }

        // Try /tmp.
        dir = "/tmp";
        if (makeFile(dir, prefix, suffix))
        {
            return;
        }

        // Give up.
        throw IOException("Couldn't find writable directory for temporary file");
    }
    
    TempFileImpl::TempFileImpl(const string& dir, const string& prefix,
                               const string& suffix)
        throw(IOException, AssertException, exception)
    {
        if (!makeFile(dir, prefix, suffix))
        {
            throw IOException("Can't write temporary file in " + dir);
        }
    }
    
    TempFileImpl::~TempFileImpl() throw()
    {
        if (!path.empty())
        {
            unlink(path.c_str());
        }
    }
    
    string TempFileImpl::getPath() throw(AssertException, exception)
    {
        return path;
    }

    string TempFileImpl::addSlash(const string& dir)
        throw(AssertException, exception)
    {
        if (dir[dir.length() - 1] == '/')
        {
            return dir;
        }
        else
        {
            return dir + '/';
        }
    }

    bool TempFileImpl::makeFile(const string& dir, const string& prefix,
                                const string& suffix)
        throw (IOException, AssertException, exception)
    {
        string base_path = addSlash(dir) ;
        pid_t process_id = getpid();

        // Increment counter until we get an unused filename.
        for (int i = 0; i < INT_MAX; ++i)
        {
			std::ostringstream ost;
            ost << base_path << prefix <<
                process_id << '_' << i <<
                suffix << ends;

#if __GNUG__ < 3
            ifstream in(ost.str(), ios::in);
#else
			ifstream in(ost.str().c_str(), std::ios_base::in);
#endif

            if (!in)
            {
                path = ost.str();
#if __GNUG__ < 3
            	ofstream out(path.c_str(), std::ios::trunc);
#else
            	ofstream out(path.c_str(), std::ios_base::trunc);
#endif
				if (!out) return false;
				else      return true;
            } else {
				in.close();
			}
        }

        throw IOException("Couldn't make unused name for temporary file");
    }
}
