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


namespace gudev {


    namespace {

        GUdevClient*
        make_filter_client(const vector<string>& subsystems)
        {
            vector<const char*> filter;
            for (auto& s : subsystems)
                filter.push_back(s.c_str());
            filter.push_back(nullptr);
            return g_udev_client_new(filter.data());
        }


        void
        dispatch_uevent_signal(GUdevClient* client_,
                               gchar*       action_,
                               GUdevDevice* device_,
                               gpointer     data)
        {
            Client* client = reinterpret_cast<Client*>(data);
            string action = action_;
            Device device = Device::view(device_);
            client->on_uevent(action, device);
        }
    }


    // Default constructor: don't listen to any events
    Client::Client() :
        Base{g_udev_client_new(nullptr), true}
    {}


    // Named constructor: listen events for subsystems
    Client::Client(const vector<string>& subsystems) :
        Base{make_filter_client(subsystems), true}
    {}


    Client::~Client()
    {
        g_signal_handler_disconnect(gobj(), uevent_handler);
    }


    gulong
    Client::connect_uevent()
    {
        return g_signal_connect(gobj(),
                                "uevent",
                                G_CALLBACK(dispatch_uevent_signal),
                                this);
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


    void
    Client::on_uevent(const string& /*action*/,
                      const Device& /*device*/)
    {}

}
