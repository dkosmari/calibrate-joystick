/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DEVICE_PAGE_HPP
#define DEVICE_PAGE_HPP

#include <memory>
#include <string>
#include <filesystem>
#include <map>

#include <gtkmm.h>
#include <libevdevxx/Device.hpp>
#include <libevdevxx/Event.hpp>
#include <libevdevxx/Code.hpp>


class App;
class AxisInfo;


class DevicePage {

    template<typename T>
    using uptr = std::unique_ptr<T>;

    template<typename T>
    using rptr = Glib::RefPtr<T>;

    std::filesystem::path dev_path;

    evdev::Device device;

    rptr<Gio::SimpleActionGroup> actions;
    rptr<Gio::SimpleAction> save_action;
    rptr<Gio::SimpleAction> delete_action;
    rptr<Gio::SimpleAction> apply_all_action;
    rptr<Gio::SimpleAction> revert_all_action;
    rptr<Gio::SimpleAction> apply_axis_action;
    rptr<Gio::SimpleAction> revert_axis_action;

    uptr<Gtk::Box> device_box;

    Gtk::Label* path_label    = nullptr;
    Gtk::Label* name_label    = nullptr;
    Gtk::Label* vendor_label  = nullptr;
    Gtk::Label* product_label = nullptr;
    Gtk::Label* version_label = nullptr;

    Gtk::CheckButton* name_check    = nullptr;
    Gtk::CheckButton* vendor_check  = nullptr;
    Gtk::CheckButton* product_check = nullptr;
    Gtk::CheckButton* version_check = nullptr;

    Gtk::Box* axes_box = nullptr;

    Gtk::InfoBar* info_bar    = nullptr;
    Gtk::Label*   error_label = nullptr;

    std::map<evdev::Code, uptr<AxisInfo>> axes;

    sigc::connection io_conn;

    bool loaded_config = false;


    void
    load_widgets();

    void
    setup_actions();

    bool
    on_io(Glib::IOCondition cond);

    void
    handle_read();


    void
    on_action_save();

    void
    on_action_delete();

    void
    on_action_apply_all();

    void
    on_action_revert_all();

    void
    on_action_apply_axis(const Glib::VariantBase& arg);

    void
    on_action_revert_axis(const Glib::VariantBase& arg);


    void
    apply_axis(evdev::Code code);

    void
    revert_axis(evdev::Code code);

    void
    disable();

    void
    try_load_config();

    // Disallow moving.
    DevicePage(DevicePage&& other) = delete;

public:

    DevicePage(const std::filesystem::path& dev_path);

    ~DevicePage();

    Gtk::Widget&
    root();

    std::string
    name()
        const;

    const std::filesystem::path&
    path()
        const;

    void
    update_colors(const App* app);


    bool
    has_loaded_config()
        const noexcept;

}; // class DevicePage

#endif
