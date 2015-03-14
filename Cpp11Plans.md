# NB++ for C++11 #

NB++ is an old library, but many ideas are in use at least in my code, but
the new C++11 standart, provide a way to construct code much more elegant
the the previous versions.

Some new simple classes will be provided like the variant and deferred,
as this make things like dynamic typing and asynchronous callback possible.

The command execute paradigm will remain, but will be converted to use the
C++ thread model.

The Indirect model used bu many classes in NB++ will be removed too, to make
the code more simple are light weigh.

This cant be done without breaking the old interface, so the future NB++
will live along the old one for a long time, as code exists that uses the
old interface and works nicely.

## Variant ##

When using C++ in a world of interaction, one discover that the strict
type system in the language are not always easy to work with. when we need
to talk to other systems.

When talking to databases, web services (xml/json), GUI libs (GTK+) or
even embedded languages, we end up using a new kind of type system every
time.
This I hope will end by adding the central Variant type and some basic
functionality for Json and DB, as inspiration (and because of some basic
needs I have too).

I hope others get inspired by this, as I think think thin may end up as
a very neat way for C++ to handle dynamic types.