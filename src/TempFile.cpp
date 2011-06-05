//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#include <nb++/TempFile.hpp>

namespace nbpp
{
    TempFile::TempFile(const string& dir, const string& prefix, const string& suffix)
        throw(IOException, AssertException, exception) :
        impl(dir.empty() ? new TempFileImpl(prefix, suffix)
             : new TempFileImpl(dir, prefix, suffix)) { }
    
    TempFile::~TempFile() throw() { }
    
    string TempFile::getPath() throw(AssertException, exception)
    {
        return impl->getPath();
    }
}
