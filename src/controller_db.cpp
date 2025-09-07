/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <compare>
#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include <set>

#include <wordexp.h>

#include <glibmm.h>

#include "controller_db.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using std::cout;
using std::cerr;
using std::endl;


namespace ControllerDB {


    /*
      Example file:

      [match]
      vendor=1a34
      product=f705
      version=0110
      name=My Weird Controller

      [ABS_X]
      min=10
      max=250
      fuzz=5
      flat=20

      [ABS_Y]
      min=10
      max=250
      fuzz=5
      flat=20

      ...
     */

    std::filesystem::path db_dir;


    struct Key {
        std::uint16_t vendor;
        std::uint16_t product;
        std::uint16_t version;
        std::string name;

        constexpr
        bool
        operator ==(const Key& other)
            const noexcept = default;

        constexpr
        std::strong_ordering
        operator <=>(const Key& other)
            const noexcept = default;
    };


    std::map<Key, DevConf> configs;
    std::map<std::filesystem::path, Key> file_to_key;


    std::filesystem::path
    get_user_config_dir()
    {
        return Glib::get_user_config_dir();
    }


    std::uint16_t
    get_hex(const Glib::KeyFile& kf,
            const Glib::ustring& group,
            const Glib::ustring& key)
    {
        if (!kf.has_key(group, key))
            return 0;
        auto str = kf.get_string(group, key);
        if (str.empty())
            return 0;
        auto result = std::stoul(str, nullptr, 16);
        if (result > std::numeric_limits<std::uint16_t>::max())
            return 0;
        return result;
    }


    void
    load_config(const std::filesystem::path& filename)
    {
        Glib::KeyFile kf;
        if (!kf.load_from_file(filename))
            throw std::runtime_error{"Could not load file."};

        if (!kf.has_group("match"))
            throw std::runtime_error{"Wrong config file: no [match] section."};

        Key key;
        key.vendor  = get_hex(kf, "match", "vendor");
        key.product = get_hex(kf, "match", "product");
        key.version = get_hex(kf, "match", "version");
        if (kf.has_key("match", "name"))
            key.name = kf.get_string("match", "name");

        DevConf conf;
        auto groups = kf.get_groups();
        for (const auto& grp : groups) {
            if (grp == "match")
                continue;
            auto& info = conf[grp];
            info.min  = kf.get_integer(grp, "min");
            info.max  = kf.get_integer(grp, "max");
            info.fuzz = kf.get_integer(grp, "fuzz");
            info.flat = kf.get_integer(grp, "flat");
        }

        auto [position, inserted] = configs.emplace(key, std::move(conf));
        if (inserted)
            file_to_key[filename] = key;
        else
            cerr << "Duplicated config in " << filename << endl;
    }


    void
    initialize()
    {
        db_dir = get_user_config_dir() / PACKAGE / "db";

        try {
            if (!exists(db_dir))
                create_directories(db_dir);

            // Parse every .conf file
            using std::filesystem::directory_options;
            using std::filesystem::directory_iterator;
            directory_iterator iter{db_dir,
                                    directory_options::follow_directory_symlink
                                    | directory_options::skip_permission_denied};
            for (const auto& entry : iter) {
                if (entry.path().extension() != ".conf")
                    continue;
                try {
                    load_config(entry.path());
                }
                catch (std::exception& e) {
                    cerr << "Failed to load " << entry.path() << ": " << e.what() << endl;
                }
            }
            cout << "Loaded " << configs.size() << " configuration(s)." << endl;
            // TODO: monitor db_dir for file changes
        }
        catch (std::exception& e) {
            cerr << "Failed to load database: " << e.what() << endl;
        }
    }


    void
    finalize()
        noexcept
    {}


    std::string
    replace_invalid_fs_chars(const std::string& input)
    {
        std::string result;
        for (unsigned char c : input) {
            if (c == '/' || c == '\0' || c < 32)
                c = '_';
            result.push_back(c);
        }
        return result;
    }


    std::filesystem::path
    make_filename(std::uint16_t vendor,
                  std::uint16_t product,
                  std::uint16_t version,
                  const std::string& name)
    {
        using Glib::ustring;
        std::string result;
        const char* sep = "";

        if (vendor) {
            result += ustring::sprintf("%04x", vendor);
            sep = "-";
        }
        if (product) {
            result += ustring::sprintf("%s%04x", sep, product);
            sep = "-";
        }
        if (version)
            result += ustring::sprintf("%s%04x", sep, version);

        auto safe_name = replace_invalid_fs_chars(name);

        if (result.empty())
            result = safe_name;
        else
            result += "(" + safe_name + ")";

        if (result.empty())
            throw std::runtime_error{"Cannot create config file with no match rules."};

        return db_dir / (result + ".conf");
    }


    void
    save(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name,
         const DevConf& configs)
    {
        // TODO: temporarily disable directory monitoring.
        using Glib::ustring;

        auto filename = make_filename(vendor, product, version, name);

        std::string vendor_str = ustring::sprintf("%04x", vendor);
        std::string product_str = ustring::sprintf("%04x", product);
        std::string version_str = ustring::sprintf("%04x", version);

        Glib::KeyFile kf;

        kf.set_comment(vendor_str + ":" + product_str + " " + version_str + " " + name);

        if (vendor)
            kf.set_string("match", "vendor", vendor_str);
        if (product)
            kf.set_string("match", "product", product_str);
        if (version)
            kf.set_string("match", "version", version_str);
        if (!name.empty())
            kf.set_string("match", "name", name);


        for (const auto& [axis, info] : configs) {
            kf.set_integer(axis, "min", info.min);
            kf.set_integer(axis, "max", info.max);
            kf.set_integer(axis, "fuzz", info.fuzz);
            kf.set_integer(axis, "flat", info.flat);
            kf.set_integer(axis, "res", info.res);
        }

        kf.save_to_file(filename);
    }


    bool
    match(const Key& a,
          const Key& b)
        noexcept
    {
        if (a.vendor && b.vendor && a.vendor != b.vendor)
            return false;
        if (a.product && b.product && a.product != b.product)
            return false;
        if (a.version && b.version && a.version != b.version)
            return false;
        if (!a.name.empty() && !b.name.empty() && a.name != b.name)
            return false;
        return true;
    }


    const DevConf*
    find(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name)
        noexcept
    {
        const Key key{ vendor, product, version, name };
        // Fast path: find an exact match.
        auto it = configs.find(key);
        if (it != configs.end())
            return &it->second;

        // Slow path: search every key using the match() function.
        for (const auto& [k, v] : configs)
            if (match(key, k))
                return &v;

        return nullptr;
    }


} // namespace ControllerDB
