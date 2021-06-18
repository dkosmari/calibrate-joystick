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

#include "app.hpp"


using std::cerr;
using std::cout;
using std::endl;

using Gio::Resource;


Glib::RefPtr<Resource>
load_resources()
{
    if (auto r = Resource::create_from_file("resources.gresource"))
        return r;

    if (auto r = Resource::create_from_file(RESOURCE_DIR "/resources.gresource"))
        return r;

    return {};
}


int main(int argc, char* argv[])
{
    auto resources = load_resources();
    if (!resources) {
        cerr << "could not load resources.gresource" << endl;
        return -1;
    }
    resources->register_global();

    App app;

    return app.run(argc, argv);
}
