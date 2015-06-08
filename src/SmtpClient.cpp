#include <nb++/SmtpClient.hpp>
#include <nb++/InetAddress.hpp>

#include <sstream>
#include <stdexcept>

namespace nbpp {
namespace Smtp {

    // Encode for the 2822 like SMTP header
    string encode_subj( const string &utf8 )
    {
        string res;
        bool encode = false;

        for( string::const_iterator i = utf8.begin(); i != utf8.end(); i++ ) {
            if( ((unsigned char)*i) > 0x7F ) {
                char buf[ 10 ];
                sprintf( buf, "=%02X", (unsigned char)*i );

                res += buf;
                encode = true;
            } else
                res += *i;
        }

        return (encode) ? "=?UTF8?Q?" + res + "?=" : res;
    }

    Message::Message( const string &mid ) : _mid( mid ) {}

    void Message::from_set( const string &email, const string &name )
    {
        _from = email;
        _from_name = name;
    }

    Strings Message::all_from_email_get() const 
    {
        Strings addr;

        addr.push_back( _to );
        
        for( rcpt_t::const_iterator i = _rcpt.begin(); i != _rcpt.end(); i++ )
            addr.push_back( i->second );

        return addr;
    }

    void Message::rcpt_add( const string &email, const string &name, addr_t type )
    {
        ostringstream os;

        if( _to.empty() && type == addr_to )
            _to = email;


        if( !name.empty())
            os << name << " <" << email << ">";
        else
            os << email;

        _rcpt.push_back( make_pair( type, os.str()));
    }

    ostream &Message::out( ostream &os ) const
    {
        char timebuf[ 40 ];
        struct tm tm;
        string cc, to;

        time_t now = time(NULL);
        localtime_r( &now, &tm );
        strftime( timebuf, sizeof( timebuf ), "%a, %-d %b %Y %T %z (%Z)", &tm );

        os << "Date: " << timebuf << "\r\n";
        os << "Message-id: " << _mid << "\r\n";

        if( _from_name.empty())
            os << "From: " << _from << "\r\n";
        else
            os << "From: " << _from_name << " <" << _from << "> " << "\r\n";

        for( rcpt_t::const_iterator i = _rcpt.begin(); i != _rcpt.end(); i++ ) {
            switch( i->first ) {
                case addr_bcc:  // Bcc is not shown in mail
                    break;

                case addr_cc:
                    if( !cc.empty())
                        cc += ", ";

                    cc += i->second;
                    break;

                case addr_to:
                    if( !to.empty())
                        to += ", ";

                    to += i->second;
                    break;

            }
        }
        if( !to.empty())
            os << "To: " << to << "\r\n";

        if( !cc.empty())
            os << "Cc: " << cc << "\r\n";

        return os;
    }

    // Impl. Utf8Message

    void Utf8Message::subject_set( const string &subj )
    {
        _subject = subj;
    }

    void Utf8Message::body_set( const string &body, const string &mimetype )
    {
        _body = body;
        _mimetype = mimetype; // text/plain
    }

    ostream &Utf8Message::out( ostream &os ) const
    {
        Message::out( os );

        os << "Subject: " << encode_subj( _subject ) << "\r\n";
        os << "Mime-Version: 1.0\r\n";
        os << "Content-Type: " << _mimetype << "; charset=UTF8;\r\n";
        os << "Content-Transfer-Encoding: 8bit\r\n";

        os << _body;
        return os;
    }

    // Impliment Client

    Client::Client( const string &dns, short port )
    {
        using namespace nbpp;

        InetAddress addr = InetAddress::getByName( dns, port );

        _sock = Socket( addr );

        ostream &os = _sock.getOutputStream();

        reply_check( 220 );

        os << "HELO SmtpClient \r\n";
        reply_check( 250 );
    }

    Client::~Client()
    {
        ostream &os = _sock.getOutputStream();

        os << "QUIT \r\n";
        reply_check( 221 );
    }

    unsigned Client::reply_check( unsigned success_code )
    {
        char reply[ 255 ];

        unsigned rcode = 0;

        ostream &os = _sock.getOutputStream();
        os.flush();
        _sock.waitToSend();

        istream &is = _sock.getInputStream();

        _sock.waitForInput();
        is.getline( reply, sizeof( reply ));
        if( 1 == sscanf( reply, "%u", &rcode )) {
            if( rcode == success_code )
                return rcode;
        }

        throw invalid_argument( reply );
    }

    bool Client::send( const Message &mail )
    {
        ostream &os = _sock.getOutputStream();

        os << "MAIL FROM: <" << mail.from_email_get() << ">\r\n";
        reply_check( 250 );

        // Send to all recipients 
        Strings recps = mail.all_from_email_get();
        for( Strings::iterator ito = recps.begin(); ito != recps.end(); ito++ ) {
            os << "RCPT TO: <" << *ito << ">\r\n";
            reply_check( 250 );
        }

        os << "DATA \r\n";
        reply_check( 354 );

        mail.out( os );

        os << "\r\n.\r\n";
        reply_check( 250 );

        return true;
    }
}
}
