/*
 *  calibrate-joystick - a program to calibrate joysticks on Linux
 *  Copyright (C) 2021  Daniel K. O.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <algorithm>

#include "utils.hpp"


using std::string;
using std::equal;

using namespace std::literals;


const string resource_prefix = "/none/calibrate_joystick/";

const string ui_main_window_path = resource_prefix + "ui/main-window.glade"s;
const string ui_device_page_path = resource_prefix + "ui/device-page.glade"s;
const string ui_axis_info_path = resource_prefix + "ui/axis-info.glade"s;
const string ui_about_dialog_path = resource_prefix + "ui/about-dialog.glade"s;


bool
starts_with(const string& haystack, const string& prefix)
{
    if (prefix.size() > haystack.size())
        return false;
    // prefix.size() <= haystack.size()
    return equal(prefix.begin(), prefix.end(),
                 haystack.begin());
}
