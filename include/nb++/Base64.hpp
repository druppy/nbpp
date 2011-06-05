/**
    Base 64 encode and decode
*/
#ifndef __BASE64_HPP__
#define __BASE64_HPP__

#include <istream>
#include <ostream>

namespace nbpp {
    using namespace std;

    /**
        Encode data from input stream into base 64 that will be inserted
        into the output stream.

        @param is any input data
        @param os base 64 encoded result.
    */
    void base64_encode( istream &is, ostream &os );

    /**
        Decode a base 64 stream, and place the result into the output
        stream.

        @param is is a stream of base 64 data
        @param os is where to put the encoded data
    */
    void base64_decode( istream &is, ostream &os );
};

#endif
