#include <algorithm>

#include "utils.hpp"


using std::string;
using std::equal;

using namespace std::literals;


const string resource_prefix = "/none/calibrate_joystick/";

const string ui_main_window_path = resource_prefix + "ui/main-window.glade"s;
const string ui_device_page_path = resource_prefix + "ui/device-page.glade"s;
const string ui_axis_info_path = resource_prefix + "ui/axis-info.glade"s;


bool
starts_with(const string& haystack, const string& prefix)
{
    if (prefix.size() > haystack.size())
        return false;
    // prefix.size() <= haystack.size()
    return equal(prefix.begin(), prefix.end(),
                 haystack.begin());
}
