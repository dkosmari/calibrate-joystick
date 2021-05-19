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

#ifndef LIBGUDEVXX_ENUMERATOR_HPP_GUARD
#define LIBGUDEVXX_ENUMERATOR_HPP_GUARD

#include <filesystem>
#include <string>
#include <vector>

#include <gudev/gudev.h>

#include "client.hpp"
#include "gobject_base.hpp"


namespace gudev {


    using std::filesystem::path;
    using std::string;
    using std::vector;


    struct Enumerator : GObjectBase<GUdevEnumerator, Enumerator> {

        using Base = GObjectBase<GUdevEnumerator, Enumerator>;


        Enumerator(Client& client);


        void match_subsystem(const string& subsystem);
        void nomatch_subsystem(const string& subsystem);

        void match_sysfs_attr(const string& key, const string& val);
        void nomatch_sysfs_attr(const string& key, const string& val);

        void match_property(const string& key, const string& val);

        void match_name(const string& name);

        void match_tag(const string& tag);

        void match_initialized();


        void add_sysfs_path(const path& sysfs_path);


        vector<Device> execute();

    };


}

#endif
