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


#ifndef GLIBMM_CHECK_VERSION
#define GLIBMM_CHECK_VERSION(major, minor, micro)                               \
    (GLIBMM_MAJOR_VERSION > (major) ||                                          \
     (GLIBMM_MAJOR_VERSION == (major) && GLIBMM_MINOR_VERSION > (minor)) ||     \
     (GLIBMM_MAJOR_VERSION == (major) && GLIBMM_MINOR_VERSION == (minor) &&     \
      GLIBMM_MICRO_VERSION >= (micro)))
#endif


using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::filesystem::path;
using std::runtime_error;
using std::string;
using std::uint16_t;

using Glib::RefPtr;


namespace ControllerDB {

    /*
      Example file:

      [match]
      vendor=1234
      product=5678
      version=9abc
      name=My Weird Controller

      [ABS_X]
      min=10
      max=250
      fuzz=5
      flat=20
      res=0
      flat_type=center

      [ABS_Y]
      min=10
      max=250
      fuzz=5
      flat=20
      res=0
      flat_type=zero

      ...
     */

    path db_dir;

    std::map<Key, DevConf> configs;


#define GLIBMM_FILE_MONITOR_IS_BROKEN

#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN
    RefPtr<Gio::FileMonitor> db_dir_monitor;
#else
    GFileMonitor* db_dir_monitor = nullptr;
#endif


    path
    get_user_config_dir()
    {
        return Glib::get_user_config_dir();
    }


    uint16_t
    get_hex(const Glib::KeyFile& kf,
            const string& group,
            const string& key)
    {
        if (!kf.has_key(group, key))
            return 0;
        string str = kf.get_string(group, key);
        if (str.empty())
            return 0;
        auto result = stoul(str, nullptr, 16);
        if (result > std::numeric_limits<uint16_t>::max())
            return 0;
        return result;
    }


    int
    get_int(const Glib::KeyFile& kf,
            const string& group,
            const string& key)
    {
        if (!kf.has_key(group, key))
            return 0;
        return kf.get_integer(group, key);
    }


    string
    get_str(const Glib::KeyFile& kf,
            const string& group,
            const string& key)
    {
        if (!kf.has_key(group, key))
            return {};
        return kf.get_string(group, key);
    }


    void
    load_config(const path& filename)
    {
        Glib::KeyFile kf;
        if (!kf.load_from_file(filename))
            throw runtime_error{"Could not load file."};

        if (!kf.has_group("match"))
            throw runtime_error{"Wrong config file: no [match] section."};

        Key key;
        key.vendor  = get_hex(kf, "match", "vendor");
        key.product = get_hex(kf, "match", "product");
        key.version = get_hex(kf, "match", "version");
        key.name    = get_str(kf, "match", "name");

        DevConf conf;
        conf.filename = filename;

        auto groups = kf.get_groups();
        for (string group : groups) {
            if (group == "match")
                continue;
            auto [type, code] = evdev::Code::parse(group);
            if (type != evdev::Type::abs)
                throw runtime_error{"Invalid axis: \"" + group + "\""};
            auto& data = conf.axes[code];
            auto& info = data.info;
            info.min  = get_int(kf, group, "min");
            info.max  = get_int(kf, group, "max");
            info.fuzz = get_int(kf, group, "fuzz");
            info.flat = get_int(kf, group, "flat");
            data.flat_centered = false;
            if (kf.has_key(group, "flat_type")) {
                auto val = kf.get_string(group, "flat_type");
                data.flat_centered = (val == "center") || (val == "centered");
            }
        }

        auto [position, inserted] = configs.emplace(key, std::move(conf));
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
            catch (exception& e) {
                cerr << "Failed to load " << entry.path() << ": " << e.what() << endl;
            }
        }
        cout << "Loaded " << configs.size() << " configuration(s)." << endl;
    }


    void
    reload_config(const path& filename)
    try {
        reload_all_configs();
    }
    catch (exception& e) {
        cerr << "Error reloading " << filename << ": " << e.what() << endl;
    }
#if !GLIBMM_CHECK_VERSION(2, 68, 0)
    catch (Glib::Exception& e) {
        cerr << "Error reloading " << filename << ": " << e.what() << endl;
    }
#endif


#ifndef GLIBMM_FILE_MONITOR_IS_BROKEN

    void
    on_db_dir_changed(const RefPtr<Gio::File>& file1,
                      const RefPtr<Gio::File>& /*file2*/,
                      Gio::FileMonitorEvent event_type)
        noexcept
    try {
        path filename = file1->get_path();

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
    catch (exception& e) {
        cout << "on_db_dir_changed(): " << e.what() << endl;
    }


#else

    void
    on_db_dir_changed(GFileMonitor* /*monitor*/,
                      GFile* file1,
                      GFile* /*file2*/,
                      GFileMonitorEvent event_type,
                      gpointer /*user_data*/)
        noexcept
    try {
        char* filename1 = g_file_get_path(file1);
        path filename = filename1;
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
                reload_config(filename);
                break;
            default:
                ;
        }
    }
    catch (exception& e) {
        cout << "on_db_dir_changed(): " << e.what() << endl;
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
        catch (exception& e) {
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


    string
    replace_invalid_fs_chars(const string& input)
    {
        string result;
        for (unsigned char c : input) {
            if (c == '/' || c == '\0' || c < 32)
                c = '_';
            result.push_back(c);
        }
        return result;
    }


    path
    make_filename(uint16_t vendor,
                  uint16_t product,
                  uint16_t version,
                  const string& name)
    {
        using Glib::ustring;
        string result;

        if (vendor || product || version)
            result += ustring::sprintf("%04x-%04x-%04x", vendor, product, version);

        auto safe_name = replace_invalid_fs_chars(name);

        if (result.empty())
            result = safe_name;
        else if (!safe_name.empty())
            result += " (" + safe_name + ")";

        if (result.empty())
            throw runtime_error{"Cannot create config file with no match rules."};

        return db_dir / (result + ".conf");
    }


    void
    save(uint16_t vendor,
         uint16_t product,
         uint16_t version,
         const string& name,
         DevConf& configs)
    {
        using Glib::ustring;

        configs.filename = make_filename(vendor, product, version, name);

        string vendor_str = ustring::sprintf("%04x", vendor);
        string product_str = ustring::sprintf("%04x", product);
        string version_str = ustring::sprintf("%04x", version);

        Glib::KeyFile kf;

        kf.set_comment(" " + vendor_str + ":" + product_str + ":" + version_str + " " + name);

        if (vendor)
            kf.set_string("match", "vendor", vendor_str);
        if (product)
            kf.set_string("match", "product", product_str);
        if (version)
            kf.set_string("match", "version", version_str);
        if (!name.empty())
            kf.set_string("match", "name", name);


        for (const auto& [axis, data] : configs.axes) {
            string group = code_to_string(evdev::Type::abs, axis);
            const auto& info = data.info;
            kf.set_integer(group, "min", info.min);
            kf.set_integer(group, "max", info.max);
            kf.set_integer(group, "fuzz", info.fuzz);
            kf.set_integer(group, "flat", info.flat);
            kf.set_integer(group, "res", info.res);
            kf.set_string(group, "flat_type",
                          data.flat_centered ? "center" : "zero");
        }

        kf.save_to_file(configs.filename);
        // Note: we don't need to add this DevConf to the controller DB, since
        // the directory monitor will load it anyway.
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


    std::pair<const Key*, const DevConf*>
    find(uint16_t vendor,
         uint16_t product,
         uint16_t version,
         const string& name)
        noexcept
    {
        const Key key{ vendor, product, version, name };
        // Fast path: find an exact match.
        if (auto it = configs.find(key); it != configs.end())
            return {&it->first, &it->second};

        // Slow path: search every key using the match() function.
        for (const auto& [k, v] : configs)
            if (match(key, k))
                return {&k, &v};

        return {nullptr, nullptr};
    }

} // namespace ControllerDB
