/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>

#include <wordexp.h>

#include <gio/gio.h>

#include <giomm.h>
#include <glibmm.h>

#include "controller_db.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using std::cout;
using std::cerr;
using std::endl;

#ifndef GLIBMM_CHECK_VERSION
#define GLIBMM_CHECK_VERSION(major, minor, micro)                               \
    (GLIBMM_MAJOR_VERSION > (major) ||                                          \
     (GLIBMM_MAJOR_VERSION == (major) && GLIBMM_MINOR_VERSION > (minor)) ||     \
     (GLIBMM_MAJOR_VERSION == (major) && GLIBMM_MINOR_VERSION == (minor) &&     \
      GLIBMM_MICRO_VERSION >= (micro)))
#endif


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
      flat_type=center

      [ABS_Y]
      min=10
      max=250
      fuzz=5
      flat=20
      flat_type=zero

      ...
     */

    std::filesystem::path db_dir;



    struct Entry {
        DevConf conf;
        std::filesystem::path filename;
    };


    std::map<Key, Entry> configs;


#define GLIBMM_FILE_MONITOR_IS_BROKEN

#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN
    Glib::RefPtr<Gio::FileMonitor> db_dir_monitor;
#else
    GFileMonitor* db_dir_monitor = nullptr;
#endif


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


    int
    get_int(const Glib::KeyFile& kf,
            const Glib::ustring& group,
            const Glib::ustring& key)
    {
        if (!kf.has_key(group, key))
            return 0;
        return kf.get_integer(group, key);
    }


    Glib::ustring
    get_str(const Glib::KeyFile& kf,
            const Glib::ustring& group,
            const Glib::ustring& key)
    {
        if (!kf.has_key(group, key))
            return {};
        return kf.get_string(group, key);
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
        key.name    = get_str(kf, "match", "name");

        Entry entry;
        auto groups = kf.get_groups();
        for (const auto& grp : groups) {
            if (grp == "match")
                continue;
            auto& data = entry.conf[grp];
            auto& info = data.info;
            info.min  = get_int(kf, grp, "min");
            info.max  = get_int(kf, grp, "max");
            info.fuzz = get_int(kf, grp, "fuzz");
            info.flat = get_int(kf, grp, "flat");
            data.flat_centered = false;
            if (kf.has_key(grp, "flat_type")) {
                auto val = kf.get_string(grp, "flat_type");
                data.flat_centered = (val == "center") || (val == "centered");
            }
        }
        entry.filename = filename;

        auto [position, inserted] = configs.emplace(key, std::move(entry));
        if (!inserted)
            cerr << "Duplicated config in " << filename << endl;
        else
            cout << "Loaded " << filename << endl;
    }


    void
    reload_all_configs()
    {
        configs.clear();

        // Parse every .conf file
        using std::filesystem::directory_options;
        using std::filesystem::directory_iterator;
        directory_iterator iter{db_dir,
                                directory_options::follow_directory_symlink
                                | directory_options::skip_permission_denied};
        for (const auto& entry : iter) {
            if (entry.path().extension() != ".conf")
                continue;
            // Ignore Emacs temporary files.
            if (entry.path().stem().string().starts_with(".#"))
                continue;
            try {
                load_config(entry.path());
            }
            catch (std::exception& e) {
                cerr << "Failed to load " << entry.path() << ": " << e.what() << endl;
            }
        }
        cout << "Loaded " << configs.size() << " configuration(s)." << endl;
    }


    void
    reload_config(const std::filesystem::path& filename)
    try {
#if 0
        // Note: this logic doesn't work well when making a copy, then deleting the original.
        for (auto& [key, entry] : configs)
            if (entry.filename == filename) {
                cout << "Unloaded " << filename << endl;
                configs.erase(key);
                break;
            }

        if (exists(filename))
            load_config(filename);
#else
        reload_all_configs();
#endif
    }
    catch (std::exception& e) {
        cerr << "Error reloading " << filename << ": " << e.what() << endl;
    }
#if !GLIBMM_CHECK_VERSION(2, 68, 0)
    catch (Glib::Exception& e) {
        cerr << "Error reloading " << filename << ": " << e.what() << endl;
    }
#endif


#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN
    void
    on_db_dir_changed(const Glib::RefPtr<Gio::File>& file1,
                      const Glib::RefPtr<Gio::File>& /*file2*/,
                      Gio::FileMonitorEvent event_type)
        noexcept
    {
        std::filesystem::path filename = file1->get_path();

        if (filename.extension() != ".conf")
            return;

        // Avoid loading Emacs temporary files.
        if (filename.stem().string().starts_with(".#"))
            return;

        switch (event_type) {
            case Gio::FILE_MONITOR_EVENT_CHANGED: // 0
            case Gio::FILE_MONITOR_EVENT_DELETED: // 2
            case Gio::FILE_MONITOR_EVENT_CREATED: // 3
                reload_config(filename);
                break;
            default:
                ;
        }
    }
#else
    void
    on_db_dir_changed(GFileMonitor* /*monitor*/,
                      GFile* file1,
                      GFile* /*file2*/,
                      GFileMonitorEvent event_type,
                      gpointer /*user_data*/)
        noexcept
    {
        try {
            char* filename1 = g_file_get_path(file1);
            std::filesystem::path filename = filename1;
            g_free(filename1);

            if (filename.extension() != ".conf")
                return;

            // Avoid loading Emacs temporary files.
            if (filename.stem().string().starts_with(".#"))
                return;

            switch (event_type) {
                case G_FILE_MONITOR_EVENT_CHANGED: // 0
                case G_FILE_MONITOR_EVENT_DELETED: // 2
                case G_FILE_MONITOR_EVENT_CREATED: // 3
                    // cout << "event_type=" << event_type << ", filename=" << filename << endl;
                    reload_config(filename);
                    break;
                default:
                    ;
            }
        }
        catch (std::exception& e) {
            cout << "on_db_dir_changed(): " << e.what() << endl;
        }
    }
#endif


    void
    initialize()
    {
        db_dir = get_user_config_dir() / PACKAGE / "db";

        try {
            if (!exists(db_dir))
                create_directories(db_dir);

            reload_all_configs();

            auto db_dir_file = Gio::File::create_for_path(db_dir);
#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN
            // This doesn't compile with glibmm 2.66.6
            db_dir_monitor = db_dir_file->monitor_directory();
            if (db_dir_monitor)
                db_dir_monitor->signal_changed().connect(on_db_dir_changed);
#else
            // Use glib directly to workaround broken wrapper.
            GFile* gfile = g_file_new_for_path(db_dir.c_str());
            db_dir_monitor = g_file_monitor_directory(gfile,
                                                      G_FILE_MONITOR_NONE,
                                                      nullptr,
                                                      nullptr);
            if (db_dir_monitor)
                g_signal_connect(db_dir_monitor,
                                 "changed",
                                 G_CALLBACK(on_db_dir_changed),
                                 nullptr);
            g_object_unref(gfile);
#endif
        }
        catch (std::exception& e) {
            cerr << "Failed to load database: " << e.what() << endl;
        }
    }


    void
    finalize()
        noexcept
    {
#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN
        db_dir_monitor.reset();
#else
        if (db_dir_monitor) {
            g_object_unref(db_dir_monitor);
            db_dir_monitor = nullptr;
        }
#endif
    }


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

        if (vendor || product || version)
            result += ustring::sprintf("%04x-%04x-%04x", vendor, product, version);

        auto safe_name = replace_invalid_fs_chars(name);

        if (result.empty())
            result = safe_name;
        else if (!safe_name.empty())
            result += " (" + safe_name + ")";

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


        for (const auto& [axis, data] : configs) {
            const auto& info = data.info;
            kf.set_integer(axis, "min", info.min);
            kf.set_integer(axis, "max", info.max);
            kf.set_integer(axis, "fuzz", info.fuzz);
            kf.set_integer(axis, "flat", info.flat);
            kf.set_integer(axis, "res", info.res);
            kf.set_string(axis, "flat_type",
                          data.flat_centered ? "center" : "zero");
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


    void
    remove(std::uint16_t vendor,
           std::uint16_t product,
           std::uint16_t version,
           const std::string& name)
    {
        const Key key{ vendor, product, version, name };
        // Fast path: find an exact match.
        if (auto it = configs.find(key); it != configs.end()) {
            auto filename = it->second.filename;
            remove(filename);
            return;
        }

        // Slow path: search every key using the match() function.
        for (auto it = configs.begin(); it != configs.end(); ++it) {
            const auto& [k, v] = *it;
            if (match(key, k)) {
                auto filename = v.filename;
                remove(filename);
                return;
            }
        }
    }


    std::pair<const Key*, const DevConf*>
    find(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name)
        noexcept
    {
        const Key key{ vendor, product, version, name };
        // Fast path: find an exact match.
        if (auto it = configs.find(key); it != configs.end())
            return {&it->first, &it->second.conf};

        // Slow path: search every key using the match() function.
        for (const auto& [k, v] : configs)
            if (match(key, k))
                return {&k, &v.conf};

        return {nullptr, nullptr};
    }


} // namespace ControllerDB
