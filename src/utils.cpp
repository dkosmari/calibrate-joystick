/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>

#include "utils.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using std::string;
using std::equal;

using namespace std::literals;



const string ui_axis_info_path   = RESOURCE_PREFIX "/ui/axis-info.glade";
const string ui_device_page_path = RESOURCE_PREFIX "/ui/device-page.glade";
