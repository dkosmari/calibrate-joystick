/*
 *  libgudevxx - a C++ wrapper for libgudev
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

#include <stdexcept>
#include <utility>

#include "client.hpp"

#include "utils.hpp"


using std::runtime_error;
using std::size_t;


namespace gudevxx {


    // Default constructor: don't listen to any events
    Client::Client() :
        Base{g_udev_client_new(nullptr), true}
    {
        if (!ptr)
            throw runtime_error{"Failed to construct GUdevClient."};
    }


    // Named constructor: listen events for subsystems
    Client
    Client::listener(const vector<string>& subsystems)
    {
        vector<const char*> arg;
        for (auto& s : subsystems)
            arg.push_back(s.c_str());
        arg.push_back(nullptr);

        return Client::own(g_udev_client_new(arg.data()));
    }



    /*------------------*/
    /* query operations */
    /*------------------*/


    vector<Device>
    Client::query(const string& subsystem)
    {
        const char* arg = subsystem.empty() ? nullptr : subsystem.c_str();
        GList* list = g_udev_client_query_by_subsystem(gobj(),
                                                       arg);
        auto vec = utils::gobj_list_to_vector<GUdevDevice*>(list);
        vector<Device> result;
        for (auto& d : vec)
            result.push_back(Device::own(d));
        return result;

    }


    optional<Device>
    Client::get(const string& subsystem,
                const string& name)
    {
        auto d = g_udev_client_query_by_subsystem_and_name(gobj(),
                                                           subsystem.c_str(),
                                                           name.c_str());
        if (d)
            return Device::own(d);
        return {};
    }


    optional<Device>
    Client::get(GUdevDeviceType type,
                uint64_t number)
    {
        auto d = g_udev_client_query_by_device_number(gobj(),
                                                      type,
                                                      number);
        if (d)
            return Device::own(d);
        return {};
    }


    optional<Device>
    Client::get(const path &device_path)
    {
        auto d = g_udev_client_query_by_device_file(gobj(),
                                                    device_path.c_str());
        if (d)
            return Device::own(d);
        return {};
    }


    optional<Device>
    Client::get_sysfs(const path &sysfs_path)
    {
        auto d = g_udev_client_query_by_sysfs_path(gobj(),
                                                   sysfs_path.c_str());
        if (d)
            return Device::own(d);
        return {};
    }

}
