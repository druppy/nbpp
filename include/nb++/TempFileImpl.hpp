//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_TEMP_FILE_IMPL
#define _NBPP_TEMP_FILE_IMPL

#include <nb++/osdep.hpp>

#include <string>
#include <fstream>

#include <nb++/RefCount.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    using std::string;

    class TempFileImpl : public RefCounted
    {
    public:
        TempFileImpl(const string& prefix, const string& suffix)
            throw(IOException, AssertException, exception);

        ~TempFileImpl() throw();

        TempFileImpl(const string& dir, const string& prefix, const string& suffix)
            throw(IOException, AssertException, exception);

        string getPath() throw(AssertException, exception);

    private:
        bool makeFile(const string& dir, const string& prefix,
                      const string& suffix) throw(IOException, AssertException, exception);
        string addSlash(const string& dir) throw(AssertException, exception);

        string path;
    };
}

#endif /* _NBPP_TEMP_FILE_IMPL */
