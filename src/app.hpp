/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APP_HPP
#define APP_HPP

// standard libraries
#include <filesystem>
#include <map>
#include <memory>
#include <string>

// system libraries
#include <gtkmm.h>

#include <gudevxx/Client.hpp>


class DevicePage;


class App : public Gtk::Application {

    Glib::RefPtr<Gio::Resource> resource;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;

    Gtk::Notebook* device_notebook = nullptr;
    Gtk::Button* quit_button = nullptr;

    std::map<std::filesystem::path,
             std::unique_ptr<DevicePage>> devices;

    std::unique_ptr<gudev::Client> uclient;

    Glib::RefPtr<Gtk::StatusIcon> status_icon;

    bool opt_daemon = false;
    bool silent_start = false;


    bool
    load_resources(const std::filesystem::path& res_path);

    void
    present_gui();

    void
    connect_uevent();

    void
    send_daemon_notification();

    void
    on_action_about();

    void
    on_action_open();

    void
    on_action_quit();

    void
    on_action_refresh();

    void
    on_activate()
        override;

    int
    on_handle_local_options(const Glib::RefPtr<Glib::VariantDict>& options);

    void
    on_open(const type_vec_files& files,
                 const Glib::ustring& hint)
        override;

    void
    on_startup()
        override;

    void
    on_uevent(const std::string& action,
              const gudev::Device& device);

public:

    App();

    ~App();


    void
    clear_devices();

    void
    add_device(const std::filesystem::path& dev_path);

    void
    remove_device(const std::filesystem::path& dev_path);


    Gdk::RGBA
    get_color_bg()
        const;

    Gdk::RGBA
    get_color_min()
        const;

    Gdk::RGBA
    get_color_max()
        const;

    Gdk::RGBA
    get_color_flat()
        const;

    Gdk::RGBA
    get_color_value()
        const;

    Gdk::RGBA
    get_color_fuzz()
        const;

};


#endif
