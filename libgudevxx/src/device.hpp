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

#ifndef LIBGUDEVXX_DEVICE_HPP_GUARD
#define LIBGUDEVXX_DEVICE_HPP_GUARD


#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <iosfwd>

#include <gudev/gudev.h>

#include "gobject_base.hpp"


namespace gudev {


    using std::chrono::microseconds;
    using std::filesystem::path;
    using std::optional;
    using std::string;
    using std::uint64_t;
    using std::vector;


    using opt_string = optional<string>;
    using opt_path = optional<path>;
    using opt_uint64_t = optional<uint64_t>;


    struct Device : GObjectBase<::GUdevDevice, Device> {


        enum class Type {
            no_device = G_UDEV_DEVICE_TYPE_NONE,
            block_device = G_UDEV_DEVICE_TYPE_BLOCK,
            char_device = G_UDEV_DEVICE_TYPE_CHAR,
        };


        using Base = GObjectBase<::GUdevDevice, Device>;


        opt_string subsystem() const;
        opt_string devtype() const;
        opt_string name() const;
        opt_string number() const;

        opt_path sysfs() const;

        opt_string driver() const;
        opt_string action() const;

        opt_uint64_t seqnum() const;

        Type type() const;

        opt_uint64_t device_number() const;

        opt_path device_file() const;

        vector<path> device_symlinks() const;

        optional<Device> parent() const;
        optional<Device> parent(const string& subsystem) const;
        optional<Device> parent(const string& subsystem,
                                const string& devtype) const;

        vector<string> tags() const;

        bool has_tag(const string& tag) const;

        bool initialized() const;

        microseconds since_initialized() const;

        vector<string> property_keys() const;

        bool has_property(const string& key) const;

        opt_string property(const string& key) const;

        // T = int, uint64_t, double, bool, string
        template<typename T>
        T property_as(const string& key) const;

        vector<string> property_tokens(const string& key) const;


        vector<string> sysfs_attr_keys() const;

        bool has_sysfs_attr(const string& key) const;

        opt_string sysfs_attr(const string& key) const;

        // T = int, uint64_t, double, bool, string
        template<typename T>
        T sysfs_attr_as(const string& key) const;

        vector<string> sysfs_attr_tokens(const string& key) const;


    private:

        using Base::Base;


    }; // class Device


    string to_string(Device::Type t);

    std::ostream& operator<<(std::ostream& out, Device::Type t);

} // namespace gudev


#endif
