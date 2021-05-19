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

#ifndef LIBGUDEVXX_CLIENT_HPP_GUARD
#define LIBGUDEVXX_CLIENT_HPP_GUARD

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <optional>

#include <gudev/gudev.h>

#include "device.hpp"
#include "gobject_base.hpp"


namespace gudev {


    using std::optional;
    using std::string;
    using std::uint64_t;
    using std::vector;
    using std::filesystem::path;


    struct Client : GObjectBase<GUdevClient, Client> {


        using Base = GObjectBase<GUdevClient, Client>;


        Client(); // don't listen on any subsystem
        Client(const vector<string>& subsystems);

        virtual ~Client();


        // query operations

        vector<Device>
        query(const string& subsystem = "");

        optional<Device>
        get(const string& subsystem,
            const string& name);

        optional<Device>
        get(GUdevDeviceType type,
            uint64_t number);

        optional<Device>
        get(const path& device_path);

        optional<Device>
        get_sysfs(const path& sysfs_path);


        virtual
        void
        on_uevent(const string& action,
                  const Device& device);


    private:

        using Base::Base;
        gulong uevent_handler = connect_uevent();

        gulong connect_uevent();

    }; // class Client


} // namespace gudev


#endif
