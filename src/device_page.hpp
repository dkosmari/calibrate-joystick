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
#include <libevdevxx/device.hpp>
#include <libevdevxx/event.hpp>


class AxisInfo;


class DevicePage {

    template<typename T>
    using ptr = std::unique_ptr<T>;

    std::filesystem::path dev_path; // TODO: should be in evdev::Device

    evdev::Device device;

    Glib::RefPtr<Gio::SimpleActionGroup> actions;

    ptr<Gtk::Box> device_box;
    ptr<Gtk::Label> name_label;
    ptr<Gtk::Label> path_label;
    ptr<Gtk::Label> axes_label;
    ptr<Gtk::Box> axes_box;

    std::map<evdev::Code, ptr<AxisInfo>> axes;

    sigc::connection io_conn;


    void load_widgets();

    bool handle_io(Glib::IOCondition cond);

    void handle_read();

    void action_apply();
    void action_reset();

    void action_apply_axis(const Glib::VariantBase& arg);
    void action_reset_axis(const Glib::VariantBase& arg);


    void apply_axis(evdev::Code code);
    void reset_axis(evdev::Code code);

public:

    DevicePage(const std::filesystem::path& dev_path);
    ~DevicePage();

    Gtk::Widget& root();

    std::string name() const;
    const std::filesystem::path& path() const;


};


#endif
