#include <algorithm>

#include "utils.hpp"


using std::string;
using std::equal;


bool
starts_with(const string& haystack, const string& prefix)
{
    if (prefix.size() > haystack.size())
        return false;
    // prefix.size() <= haystack.size()
    return equal(prefix.begin(), prefix.end(),
                 haystack.begin());
}
