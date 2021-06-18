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


#include <iostream>

#include "joystick_listener.hpp"

#include "app.hpp"
#include "utils.hpp"


using std::string;
using std::cout;
using std::endl;

using gudev::Client;
using gudev::Device;


JoystickListener::JoystickListener(App& app) :
    Client{{"input"}},
    app(app)
{}


void
JoystickListener::on_uevent(const string& action,
                            const Device& device)
{
    if (auto subsystem = device.subsystem();
        !subsystem || *subsystem != "input")
        return;

    if (auto name = device.name();
        !name || !starts_with(*name, "event"))
        return;

    if (!device.property_as<bool>("ID_INPUT_JOYSTICK"))
        return;

    if (!device.has_tag("uaccess"))
        return;

    if (action == "add")
        app.add_device(device);
    else if (action == "remove")
        app.remove_device(device);
    else
        cout << "ignoring action: " << action << endl;
}
