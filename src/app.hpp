/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APP_HPP
#define APP_HPP

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include <gtkmm.h>

#include <gudevxx/Client.hpp>


class DevicePage;
class Settings;


class App : public Gtk::Application {

    Glib::RefPtr<Gio::Resource> resource;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;
    std::unique_ptr<Gtk::AboutDialog> about_window;
    std::unique_ptr<Settings> settings_window;

    Gtk::Notebook* device_notebook = nullptr;
    Gtk::Button* quit_button = nullptr;

    std::map<std::filesystem::path,
             std::unique_ptr<DevicePage>> devices;

    gudev::Client uclient = nullptr;

    Glib::RefPtr<Gtk::StatusIcon> status_icon;

    bool opt_daemon = false;
    bool silent_start = false;

    Gdk::RGBA background_color;
    Gdk::RGBA value_color;
    Gdk::RGBA min_color;
    Gdk::RGBA max_color;
    Gdk::RGBA fuzz_color;
    Gdk::RGBA flat_color;


    bool
    load_resources(const std::filesystem::path& res_path);

    void
    load_gui();

    void
    present_main_window();

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
    on_action_settings();


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


    void
    update_colors();

public:

    App();

    ~App();


    void
    clear_devices();

    void
    add_device(const std::filesystem::path& dev_path);

    void
    remove_device(const std::filesystem::path& dev_path);


    const Gdk::RGBA&
    get_background_color()
        const noexcept;

    const Gdk::RGBA&
    get_value_color()
        const noexcept;

    const Gdk::RGBA&
    get_min_color()
        const noexcept;

    const Gdk::RGBA&
    get_max_color()
        const noexcept;

    const Gdk::RGBA&
    get_fuzz_color()
        const noexcept;

    const Gdk::RGBA&
    get_flat_color()
        const noexcept;


    void
    set_background_color(const Gdk::RGBA& color);

    void
    set_value_color(const Gdk::RGBA& color);

    void
    set_min_color(const Gdk::RGBA& color);

    void
    set_max_color(const Gdk::RGBA& color);

    void
    set_fuzz_color(const Gdk::RGBA& color);

    void
    set_flat_color(const Gdk::RGBA& color);

};


#endif
