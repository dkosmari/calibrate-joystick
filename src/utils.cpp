#include <algorithm>

#include "utils.hpp"


using std::string;
using std::equal;


const char* ui_main_window_path = "ui/main-window.glade";
const char* ui_device_page_path = "ui/device-page.glade";


bool
starts_with(const string& haystack, const string& prefix)
{
    if (prefix.size() > haystack.size())
        return false;
    // prefix.size() <= haystack.size()
    return equal(prefix.begin(), prefix.end(),
                 haystack.begin());
}
