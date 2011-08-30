/**
    This will end up as a part of nb++, to be used for both SMTP and
    file upload.

    There are no content handling at all, at the moment so if content
    are in base 64, it need explicit conversion.

    http://www.ietf.org/rfc/rfc1867.txt
    http://www.ietf.org/rfc/rfc2388.txt
*/

#ifndef _MULTIPART_HPP_
#define _MULTIPART_HPP_ 1

#include <string>
#include <vector>
#include <fstream>

#include <nb++/HTTPServer.hpp>

namespace nbpp {

using namespace std;

class Multipart;

class Part {
    friend class Multipart;

    string _name,
        _filename;

    typedef vector<pair<string, string> > headers_t;
    headers_t _headers;

    void set( const string &name, const string &value );

protected:
    virtual bool put( char ch ) = 0;
    virtual void eof() = 0;

public:
    Part() {}
    Part( const Part &other );

    virtual ~Part() {}

    virtual size_t size() const = 0;

    string get( const string &name ) const;

    string name_get() const {return _name;}

    string filename_get() const {return _filename;}

    // Sugar
    string operator[] ( const string &name ) const {return get( name );}
};

/**
    A multi part element, the header elements are part of the part data
    and the stream may be a file or string stream depending on content.
*/
class FilePart : public Part {
    FILE *_file;
    size_t _size;

protected:
    bool put( char ch );
    void eof();

public:
    FilePart();
    FilePart( const Part &other );
    ~FilePart();

    size_t size() const {return _size;}

    size_t out( ostream &os );
};

class MemPart : public Part
{
    string _buf;
protected:
    bool put( char ch ) {
        _buf += ch;
        return true;
    }

    void eof() {_buf = _buf.substr( 0, _buf.size() - 2 );}

public:
    MemPart() {}
    MemPart( const Part &other ) : Part( other ) {}

    size_t size() const {return _buf.size();}

    string str() {return _buf;}
};

/**
    Read the multipart data from the istream provided in the constructor
    and for each part found make a MimePart containíng the header and
    the content ad a stream.
*/
class Multipart {
    string _boundary;
    typedef vector<Part *> parts_t;
    parts_t _parts;

    bool parse( istream &is );
public:
    Multipart( HTTPRequest &req );
    Multipart( istream &is, const string &boundary );

    ~Multipart();

    size_t size() const { return _parts.size();}

    Part &part( size_t idx ) {return *_parts[ idx ];}

    // Sugar !
    Part &operator []( size_t idx ) {return part( idx );}
};

}

#endif
