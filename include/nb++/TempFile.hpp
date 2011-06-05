//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_TEMP_FILE
#define _NBPP_TEMP_FILE

#include <nb++/TempFileImpl.hpp>

namespace nbpp
{
    /**
     * When constructed, a TempFile creates a temporary file on the filesystem.  The
     * filename is guaranteed to be unique in a multithreaded program.  The file is
     * deleted when the last copy of the corresponding TempFile object goes out of scope.
     *
     * TempFile won't work properly if you tell it to create files in an NFS-mounted
     * directory.
     *
     * This class is reference-counted.
     */
    class TempFile
    {
    public:
        /**
         * Creates a temporary file.
         *
         * If a directory was specified (non-empty string), then that directory is used.
         * Else, the file is placed in the directory specified by the environment variable
         * TMPDIR (if defined and writable), otherwise in /tmp.
         *
         * @param dir the directory in which to create the file.
         * @param prefix the prefix of the filename.
         * @param suffix the suffix of the filename.
         * @exception IOException if an I/O error occurs.
         */
        TempFile(const string& dir = "", const string& prefix = "",
                 const string& suffix = ".tmp")
            throw(IOException, AssertException, exception);

        /**
         * If this is the last remaining TempFile object referring to the temporary file,
         * the destructor deletes the file.
         */
        ~TempFile() throw();

        string getPath() throw(AssertException, exception);

    private:
        RefHandle<TempFileImpl> impl;
    };
}

#endif /* _NBPP_TEMP_FILE */
