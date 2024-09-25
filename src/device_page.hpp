/*
 *  calibrate-joystick - a program to calibrate joysticks on Linux
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
    uptr<Gtk::Label> axes_label;
    uptr<Gtk::Box> axes_box;
    uptr<Gtk::InfoBar> info_bar;
    uptr<Gtk::Label> error_label;

    std::map<evdev::Code, uptr<AxisInfo>> axes;

    sigc::connection io_conn;


    void load_widgets();

    bool on_io(Glib::IOCondition cond);

    void handle_read();

    void on_action_apply();
    void on_action_reset();

    void on_action_apply_axis(const Glib::VariantBase& arg);
    void on_action_reset_axis(const Glib::VariantBase& arg);


    void apply_axis(evdev::Code code);
    void reset_axis(evdev::Code code);

    void disable();

public:

    DevicePage(const std::filesystem::path& dev_path);
    ~DevicePage();

    Gtk::Widget& root();

    std::string name() const;
    const std::filesystem::path& path() const;


};


#endif
