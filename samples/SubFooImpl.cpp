#include "SubFooImpl.hpp"
#include <iostream>

SubFooImpl::SubFooImpl(const string& v) : value(v)
{
    cout << "SubFooImpl \"" << value << "\" constructed." << endl;
}

SubFooImpl::~SubFooImpl()
{
    cout << "SubFooImpl \"" << value << "\" destructor called." << endl;
}

string SubFooImpl::getValue()
{
    return value;
}
