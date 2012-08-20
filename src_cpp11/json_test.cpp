#include "json.hpp"
#include <sstream>
// 1 2 3 testing

#include <iostream>
#include <fstream>

using namespace std;
using namespace nbpp;

void test( )
{
    stringstream os;

    os << "[23,5.6,null,{\"haæøåsh\":true,\"n\bumber\":42},[false,\"one\",\"two\",\"tree\"]]";

	Variant nil;
    Variant res = json_parse( os );

    Variant r = res.get<nbpp::cvector>()[ 2 ];

    cout << "null : " << r << ", " << nil << endl;
    cout << "bool " << typeid( bool ).name() << "int" << typeid( int ).name() << endl;

    cout << "original " << os.str() << endl;
    cout << "parsed : " << res << endl;
    
    cout << "json serialized : "; json_serialize( cout, res ) << endl; 
}

int main( int argc, char *argv[] ) 
{
    Variant res;
    
    if( argc > 1 ) {
        ifstream os( argv[ 1 ] );
        
        res = json_parse( os );
    } else
        res = json_parse( cin );
    
    json_serialize( cout, res ) << endl;
}
