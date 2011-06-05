#include <ConfigFile.hpp>
#include <ExceptionGuard.hpp>

/**
 * Tests ConfigFile.
 */

using namespace std;
using namespace nbpp;

int main(int argc, char* argv[])
{
	ExceptionGuard exceptionGuard;

	try
    {
		ConfigFile cf(cin);

		cf ["foobar"]["bar"] = "foobaz";

		cout << cf ["foo"]["bar"] << endl;
		cout << cf ["foobar"]["bar"] << endl;

		cf ["foobaz"]["baz"] = cf ["foobar"]["bar"];
		cout << cf ["foobaz"]["baz"] << endl;

		cf ["baz"] = cf ["foo"];
		cf ["baz"]["hello"] = "testing";

		cout << cf ["baz"]["bar"] << endl;

		cf.findFirstMatch("foo", "bar", "foo") ["foobaz"] = "foo";
		cf.findFirstMatch("foo", "bar", "bar") ["foobaz"] = "bar";
		cf.findFirstMatch("foo", "bar", "baz") ["foobaz"] = "baz";
		cf.save (cout);
	}
    catch (Exception& e)
    {
		cerr << e.toString() << endl;
		return 1;
	}
    catch (exception& e)
    {
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}
