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

    std::filesystem::path dev_path; // TODO: should be in evdev::Device

    evdev::Device device;

    rptr<Gio::SimpleActionGroup> actions;
    rptr<Gio::SimpleAction> apply_action;
    rptr<Gio::SimpleAction> reset_action;
    rptr<Gio::SimpleAction> apply_axis_action;
    rptr<Gio::SimpleAction> reset_axis_action;


    uptr<Gtk::Box> device_box;
    uptr<Gtk::Label> name_label;
    uptr<Gtk::Label> path_label;
    uptr<Gtk::Label> vid_pid_label;
    uptr<Gtk::Label> axes_label;
    uptr<Gtk::Box> axes_box;
    uptr<Gtk::InfoBar> info_bar;
    uptr<Gtk::Label> error_label;

    std::map<evdev::Code, uptr<AxisInfo>> axes;

    sigc::connection io_conn;


    void
    load_widgets();

    bool
    on_io(Glib::IOCondition cond);

    void
    handle_read();

    void
    on_action_apply();

    void
    on_action_reset();

    void
    on_action_apply_axis(const Glib::VariantBase& arg);

    void
    on_action_reset_axis(const Glib::VariantBase& arg);


    void
    apply_axis(evdev::Code code);

    void
    reset_axis(evdev::Code code);

    void
    disable();

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

}; // class DevicePage

#endif
