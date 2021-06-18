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


#ifndef APP_HPP
#define APP_HPP

// standard libraries
#include <memory>
#include <string>
#include <vector>

// system libraries
#include <gtkmm.h>
#include <gudevxx/client.hpp>

// local headers
#include "joystick_listener.hpp"


class DevicePage;


class App : public Gtk::Application {

    JoystickListener joystick_listener;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;

    Gtk::Notebook* device_notebook = nullptr;

    std::vector<std::unique_ptr<DevicePage>> devices;


    void on_startup() override;

    void on_activate() override;

public:

    App();
    ~App();


    void create_main_window();


    void clear_devices();

    void add_device(const gudev::Device& d);

    void remove_device(const gudev::Device& d);


    void refresh();

};


#endif
