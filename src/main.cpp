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
#include <stdexcept>

#include "app.hpp"


using std::cerr;
using std::endl;

using Gio::Resource;


bool
load_resources(const std::filesystem::path& res_path)
{
    try {
        g_debug("trying to load resources from \"%s\"", res_path.c_str());
        auto r = Resource::create_from_file(res_path);
        if (!r)
            return false;
        g_debug("loaded resource file from \"%s\"", res_path.c_str());
        r->register_global();
        return true;
    }
    catch (...) {
        return false;
    }
}


int main(int argc, char* argv[])
{
    try {
        if (!load_resources("resources.gresource") &&
            !load_resources(RESOURCE_DIR "/resources.gresource"))
            throw std::runtime_error{"could not load resource file."};

        App app;
        return app.run(argc, argv);
    }
    catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }
}
