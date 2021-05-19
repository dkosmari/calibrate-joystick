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

#include <algorithm>

#include "device.hpp"

#include "utils.hpp"


namespace gudev {

    opt_string
    Device::subsystem() const
    {
        auto r = g_udev_device_get_subsystem(gobj());
        if (r)
            return r;
        return {};
    }


    opt_string
    Device::devtype() const
    {
        auto r = g_udev_device_get_devtype(gobj());
        if (r)
            return r;
        return {};
    }


    opt_string
    Device::name() const
    {
        auto r = g_udev_device_get_name(gobj());
        if (r)
            return r;
        return {};
    }


    opt_string
    Device::number() const
    {
        auto r = g_udev_device_get_number(gobj());
        if (r)
            return r;
        return {};
    }


    opt_path
    Device::sysfs() const
    {
        auto r = g_udev_device_get_sysfs_path(gobj());
        if (r)
            return r;
        return {};
    }


    opt_string
    Device::driver() const
    {
        auto r = g_udev_device_get_driver(gobj());
        if (r)
            return r;
        return {};
    }


    opt_string
    Device::action() const
    {
        auto r = g_udev_device_get_action(gobj());
        if (r)
            return r;
        return {};
    }


    opt_uint64_t
    Device::seqnum() const
    {
        auto s = g_udev_device_get_seqnum(gobj());
        if (s)
            return s;
        return {};
    }


    Device::Type
    Device::type() const
    {
        auto t = g_udev_device_get_device_type(gobj());
        return Type{t};
    }


    opt_uint64_t
    Device::device_number() const
    {
        auto n = g_udev_device_get_device_number(gobj());
        if (n)
            return n;
        return {};
    }


    opt_path
    Device::device_file() const
    {
        auto f = g_udev_device_get_device_file(gobj());
        if (f)
            return f;
        return {};
    }


    vector<path>
    Device::device_symlinks() const
    {
        auto a = g_udev_device_get_device_file_symlinks(gobj());
        return utils::strv_to_vector<path>(a);
    }


    optional<Device>
    Device::parent() const
    {
        auto p = g_udev_device_get_parent(gobj());
        if (p)
            return Device::own(p);
        return {};
    }


    optional<Device>
    Device::parent(const string& subsystem) const
    {
        auto p = g_udev_device_get_parent_with_subsystem(gobj(),
                                                         subsystem.c_str(),
                                                         nullptr);
        if (p)
            return Device::own(p);
        return {};
    }


    optional<Device>
    Device::parent(const string& subsystem,
                   const string& devtype) const
    {
        auto p = g_udev_device_get_parent_with_subsystem(gobj(),
                                                         subsystem.c_str(),
                                                         devtype.c_str());
        if (p)
            return Device::own(p);
        return {};
    }


    vector<string>
    Device::tags() const
    {
        auto t = g_udev_device_get_tags(gobj());
        return utils::strv_to_vector(t);
    }


    bool
    Device::has_tag(const string& tag) const
    {
        auto list = tags();
        return find(list.begin(), list.end(), tag) != list.end();
    }


    bool
    Device::initialized() const
    {
        return g_udev_device_get_is_initialized(gobj());
    }


    microseconds
    Device::since_initialized() const
    {
        auto u = g_udev_device_get_usec_since_initialized(gobj());
        return microseconds(u);
    }



    vector<string>
    Device::property_keys() const
    {
        auto k = g_udev_device_get_property_keys(gobj());
        return utils::strv_to_vector(k);
    }


    bool
    Device::has_property(const string& key) const
    {
        return g_udev_device_has_property(gobj(), key.c_str());
    }


    opt_string
    Device::property(const string& key) const
    {
        auto p = g_udev_device_get_property(gobj(), key.c_str());
        if (p)
            return p;
        return {};
    }


    template<>
    int
    Device::property_as<int>(const string& key) const
    {
        return g_udev_device_get_property_as_int(gobj(), key.c_str());
    }


    template<>
    uint64_t
    Device::property_as<uint64_t>(const string& key) const
    {
        return g_udev_device_get_property_as_uint64(gobj(), key.c_str());
    }


    template<>
    double
    Device::property_as<double>(const string& key) const
    {
        return g_udev_device_get_property_as_double(gobj(), key.c_str());
    }


    template<>
    bool
    Device::property_as<bool>(const string& key) const
    {
        return g_udev_device_get_property_as_boolean(gobj(), key.c_str());
    }


    template<>
    string
    Device::property_as<string>(const string& key) const
    {
        return property(key).value_or("");
    }


    vector<string>
    Device::property_tokens(const string& key) const
    {
        auto t = g_udev_device_get_property_as_strv(gobj(), key.c_str());
        return utils::strv_to_vector(t);
    }


    vector<string>
    Device::sysfs_attr_keys() const
    {
        auto k = g_udev_device_get_sysfs_attr_keys(gobj());
        return utils::strv_to_vector(k);
    }


    bool
    Device::has_sysfs_attr(const string& key) const
    {
        return g_udev_device_has_sysfs_attr(gobj(), key.c_str());
    }


    opt_string
    Device::sysfs_attr(const string& key) const
    {
        auto a = g_udev_device_get_sysfs_attr(gobj(), key.c_str());
        if (a)
            return a;
        return {};
    }


    template<>
    int
    Device::sysfs_attr_as<int>(const string& key) const
    {
        return g_udev_device_get_sysfs_attr_as_int(gobj(), key.c_str());
    }


    template<>
    uint64_t
    Device::sysfs_attr_as<uint64_t>(const string& key) const
    {
        return g_udev_device_get_sysfs_attr_as_uint64(gobj(), key.c_str());
    }


    template<>
    double
    Device::sysfs_attr_as<double>(const string& key) const
    {
        return g_udev_device_get_sysfs_attr_as_double(gobj(), key.c_str());
    }


    template<>
    bool
    Device::sysfs_attr_as<bool>(const string& key) const
    {
        return g_udev_device_get_sysfs_attr_as_boolean(gobj(), key.c_str());
    }


    template<>
    string
    Device::sysfs_attr_as<string>(const string& key) const
    {
        return sysfs_attr(key).value_or("");
    }


    vector<string>
    Device::sysfs_attr_tokens(const string& key) const
    {
        auto t = g_udev_device_get_sysfs_attr_as_strv(gobj(), key.c_str());
        return utils::strv_to_vector(t);
    }



    string
    to_string(Device::Type t)
    {
        switch (t) {
            case Device::Type::no_device:
                return "no device file";
            case Device::Type::block_device:
                return "block_device";
            case Device::Type::char_device:
                return "character device";
            default:
                throw std::logic_error{"invalid device type"};
        }
    }


    std::ostream&
    operator<<(std::ostream& out, Device::Type t)
    {
        return out << to_string(t);
    }


}
