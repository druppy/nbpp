#include <nb++/Multipart.hpp>
#include <nb++/String.hpp>
#include <iostream>
#include <cstdlib>


using namespace nbpp;

Part::Part( const Part &other )
{
    _name = other._name;
    _filename = other._filename;
    _headers = other._headers;
}

void Part::set( const string &name, const string &value )
{
    _headers.push_back(make_pair( name, value ));

    if( name == "Content-Disposition" ) {
        char name[ 100 ], filename[ 100 ];

        int cnt = sscanf( value.c_str(), "form-data; name=\"%[^\"]\"; filename=\"%[^\"]\"", name, filename );
        if( cnt == 2 )
            _filename = filename;

        if( cnt > 0 )
            _name = name;
    }
}

string Part::get( const string &name ) const
{
    headers_t::const_iterator i;
    for( i = _headers.begin(); i != _headers.end(); i++ ) {
        if( i->first == name )
            return i->second;
    }

    return "";
}

FilePart::FilePart() : Part(), _size( 0 )
{
    _file = tmpfile();
}

FilePart::FilePart( const Part &other ) : Part( other ), _size( 0 )
{
    _file = tmpfile();
}

FilePart::~FilePart()
{
    fclose( _file );
}

void FilePart::eof()
{
    _size -= 2; // remove cr/nl
}

bool FilePart::put( char ch )
{
    if( EOF != fputc( ch, _file )) {
        ++_size;
        return true;
    }

    return false;
}

size_t FilePart::out( ostream &os )
{
    char buf[ 256 ];
    size_t total = 0, read = _size;

    rewind( _file );

    while( 0 == feof( _file ) && read > 0) {
        size_t size = fread( buf, 1,
            (read > sizeof( buf )) ? sizeof( buf ) : read,
            _file );

        os.write( buf, size );
        total += size;
        read -= size;
    }
    return total;
}

///////////////////
// Multipart

Multipart::Multipart( nbpp::HTTPRequest &req )
{
    istream &is = req.getInputStream();

    string type = req[ "Content-Type" ];

    if( type.substr( 0, 19 ) == "multipart/form-data" ) {
        string::size_type pos = type.find( ';' );

        string boundary = trim( type.substr( pos + 1, string::npos ));

        pos = boundary.find( '=' );

        if( pos != string::npos && boundary.substr( 0, pos ) == "boundary" ) {
            _boundary = boundary.substr( pos + 1, string::npos );

            parse( is );
        }
    }
}

Multipart::Multipart( istream &is, const string &boundary ) : _boundary( boundary )
{
    parse( is );
}

Multipart::~Multipart()
{
    for( parts_t::iterator i = _parts.begin(); i != _parts.end(); i++ )
        delete *i;
}

bool Multipart::parse( istream &is )
{
    string line, fullboundary = "--" + _boundary;
    bool the_end = false;

    Part *part = NULL;

    size_t boundary_pos = 0;
    const char *boundary = fullboundary.c_str();
    while( !the_end ) {
        int ch = is.get();

        if( !is || ch != -1 )
            break;

        if( ch == boundary[ boundary_pos ] ) {
            if( boundary[ ++boundary_pos ] == 0 ) {
                boundary_pos = 0;

                do {
                    ch = is.get();
                } while( isspace( ch ) && ch != -1 && is );

                if( !is )
                    break;

                if( part )
                    part -> eof();

                if( '-' == ch ) {
                    ch = is.get();

                    if( ch == -1 || !is )
                        break;

                    if( '-' == ch ) {
                        the_end = true;

                        // Consume leftover whitespace
                        do {
                            ch = is.get();
                        } while( isspace( ch ) && ch != -1 && is);
                        clog << "---- end of multipart ---" << endl;

                        break;
                    } else
                        is.unget();
                } else
                    is.unget();

                part = new MemPart();

                // Read headers
                while( !is.eof()) {
                    getline( is, line );

                    line = trim( line );

                    if( line.empty())
                        break;

                    Strings head = split( line, ':' );
                    part->set( trim( head[0] ), trim( head[1] ));
                }

                if( !part->filename_get().empty()) {
                    Part *old_part = part;

                    part = new FilePart( *old_part ); // We expect a large data chunk
                    delete old_part;
                }

                _parts.push_back( part );
            }
            continue;
        } else {
            if( boundary_pos > 0 ) {
                if( part )
                    for( size_t n = 0; n != boundary_pos; n++ )
                        if( !part->put( boundary[ n ] ))
                            break;

                boundary_pos = 0;
            }
        }

        if( part )
            if( !part->put( ch ))
                the_end = true;
    }

    return true;
}
