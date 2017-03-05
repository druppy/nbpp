/**
    Define a simple SMTP mail client handler, that is simple to expand
    to more complex handling.

    The current system only accept a well working SMTP MTA, and will throw
    an exception in the first site of trouble.
*/
#ifndef _SMTP_CLIENT_HPP
#define _SMTP_CLIENT_HPP

#include <list>
#include <string>
#include <nb++/Socket.hpp>
#include <nb++/String.hpp>

namespace nbpp {
namespace Smtp {

using namespace std;

/**
    Define the content of a single smtp mail, by defining both the header
    elements and the body content.

    This will be able to send a single mail with a empty body element.

    @see Client
*/
class Message {
    string _from, _from_name,
        _to,
        _mid;
public:
    enum addr_t {
        addr_to,
        addr_cc,
        addr_bcc
    };
    typedef list<pair<addr_t, string> > rcpt_t;

    /**
        Construct a new Mail, holding the all data needed to send a single
        message.

        Note that this object does not have any body element, so to be able
        to send body content, use special enherits.

        @param mid unique message id
    */
    Message( const string &mid );

    /**
        Define the from address and the real name.

        @param email holds the email
        @param name of the email holder
    */
    void from_set( const string &email, const string &name );

    /**
        Define the recipient's of this message. These recipients can be
        one of 3 types.

        The first address of the type addr_to, will be the primary, and
        this is mandatory.

        @param email holds the email of the recipient
        @param name holds the name of the recipient
        @param type holds the type (addr_to, addr_cc, addr_bcc)
    */
    void rcpt_add( const string &email, const string &name, addr_t type = addr_to );

    string from_email_get() const {return _from;}
    string to_email_get() const {return _to;}

    // return all adresses to send to
    Strings all_from_email_get() const;

    /**
        Used by the Client to serialize the content of this message in the
        rfc822 way.

        @param os the stream to send this to
        @return is the same stream as given as param
    */
    virtual ostream &out( ostream &os ) const;
private:
    rcpt_t _rcpt;
};

/**
    Mail body that sends out pure Utf8 content, as the body.

    Note that some old MTAø's may only accept 7bit content.

    @see Client
    @see Message
*/
class Utf8Message : public Message
{
    string _body, 
    _subject,
    _mimetype;
public:
    /**
        Construct a special message that handle the body content as UTF8,
        pure 8bit. Not all MTA's may accept this !

        @param mid holds the message id
    */
    Utf8Message( const string &mid ) : Message( mid ) {}

    /**
        Set the subject of this message.

        @param subj holds the subject of the message (utf8)
    */
    void subject_set( const string &subj );

    /**
        Set the body content of this message, and this class need the content
        of this body string to be in UTF8.

        @param body content of the message
        @param mimetype of the given body (all is utf8)
    */
    void body_set( const string &body, const string &mimetype = "text/plain" );

    ostream &out( ostream &os ) const;
};

/**
    This smtp client class, will try to open a connection to a SMTP MTA,
    to be ready to send mails. This connection will be open for as long
    as the this class instance are active.

    This class handles the SMTP protocol in a conservative way, and all
    exceptions from the normal flow will regarded as an error and the
    unexpected SMTP response will be part of the throwned exceptions
    what text.

    This class is NOT reentrant, to use this in a threaded env. please
    make a instance per thread.
*/
class Client {
    nbpp::Socket _sock;
protected:
    unsigned reply_check( unsigned success_code );

public:
    /**
        Construct a client connection to a MTA, and keep it open for as
        long as this instance keeps existing.

        @param dns holds the dns or IP of the MTA
        @param port hold the port used to access to MTA (default 25)
    */
    Client( const string &dns, short port = 25 );

    /**
        Close down the MTA connection
    */
    ~Client();

    /**
        Send a single mail to the open MTA connection, and handle the
        SMTP protocol, to ensure this goes as expected.

        @param mail is a ref to a mail instance
        @return true upon success
    */
    bool send( const Message &mail );
};

}
}
#endif
