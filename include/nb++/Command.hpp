//  nb++, a nuts-and-bolts C++ toolkit.
//  Copyright (C) 1999 Benjamin Geer, Eduardo J. Blanco

#ifndef _NBPP_COMMAND
#define _NBPP_COMMAND

#include <nb++/RefCount.hpp>
#include <nb++/Exception.hpp>

namespace nbpp
{
    /**
     * An interface for objects implementing the Command design pattern.
     * (See <em>Design Patterns</em>, Gamma et al.)
     *
     * @see Command
     */
    class CommandImpl : public RefCounted
    {
    public:
        /**
         * Destructor.
         */
        virtual ~CommandImpl() throw();

        /**
         * Executes the command.
         */
        virtual void execute() = 0;
    };

    /**
     * A reference-counted proxy for a CommandImpl.
     *
     * @see CommandImpl
     */
    class Command
    {
    public:
        /**
         * Wraps a CommandImpl.
         */
        Command(CommandImpl* impl) throw(AssertException, exception);

        /**
         * Calls the execute() method of the CommandImpl.  This method may throw any
         * exception, but if it is called by a nb++ class, any exceptions it throws
         * may be caught and ignored.
         */
        void execute();

        /**
         * Constructs a null Command.
         */
        Command() throw(exception);

        /**
         * @return true if this object is non-null.
         */
        operator bool() const throw();

    private:
        RefHandle<CommandImpl> impl;
    };

    /**
     * A CommandImpl whose execute() method calls a method on some other object, passing
     * it a single argument.  The method should be of the form:
     *
     * <pre>
     * void foo(Data&);
     * </pre>
     *
     * @see CommandImpl
     * @see Command
     */
    template<class Data, class Receiver> class OneArgCommandImpl : public CommandImpl
    {
    public:
        /**
         * @param arg_data the argument to be passed to arg_pm when execute() is
         * called.
         * @param arg_receiver the object on which arg_pm should be called.
         * @param arg_pm the method of arg_receiver that should be called when
         * execute() is called.
         */
        OneArgCommandImpl(Data& arg_data, Receiver* arg_receiver,
                          void (Receiver::* arg_pm)(Data&))
            throw(AssertException, exception) :
            data(arg_data), receiver(arg_receiver), pm(arg_pm) { }

        /**
         * Destructor.
         */
        virtual ~OneArgCommandImpl() throw() { }

        /**
         * Calls the method specified by the constructor.
         */
        virtual void execute()
        {
            (receiver->*pm)(data);
        }
        
    private:
        Data data;
        Receiver* receiver;
        void (Receiver::* pm)(Data&);
    };
}

#endif /* _NBPP_COMMAND */
