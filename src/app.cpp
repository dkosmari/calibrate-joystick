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


#include <iostream>

#include <gudevxx/enumerator.hpp>

#include "app.hpp"

#include "device_page.hpp"
#include "utils.hpp"


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::make_unique;

using UDevice = gudev::Device;
using gudev::Enumerator;

using Glib::RefPtr;


App::App() :
    Gtk::Application{"none.calibrate_joystick"},
    joystick_listener{*this}
{}


App::~App() = default;


void
App::create_main_window()
{
    auto builder = Gtk::Builder::create_from_resource(ui_main_window_path);

    main_window = get_widget<Gtk::ApplicationWindow>(builder,
                                                     "main_window");
    header_bar = get_widget<Gtk::HeaderBar>(builder,
                                            "header_bar");
    main_window->set_titlebar(*header_bar);

    //builder->get_widget_derived("refresh_button", refresh_button, *this);

    builder->get_widget("device_notebook", device_notebook);

    add_window(*main_window);
}


void
App::on_startup()
{
    //cout << "on_startup()" << endl;

    Gtk::Application::on_startup();

    add_action("about", [this]
    {
        auto builder = Gtk::Builder::create_from_resource(ui_about_dialog_path);
        auto about_dialog = get_widget<Gtk::AboutDialog>(builder, "about_dialog");
        about_dialog->set_application(main_window->get_application());
        about_dialog->set_version(PACKAGE_VERSION);
        about_dialog->run();
    });

    add_action("quit", sigc::mem_fun(this, &App::quit));

    add_action("refresh", sigc::mem_fun(this, &App::refresh));

    add_action("apply", []{ cout << "APPLY" << endl; });
}


void
App::on_activate()
{
    //cout << "on_activate()" << endl;

    create_main_window();

    main_window->show_all();
    main_window->present();

    refresh();
}


void
App::clear_devices()
{
    devices.clear();
}


void
App::add_device(const UDevice& device)
{
    auto dev_path = device.device_file().value();
    //cout << "adding " << dev_path << endl;
    devices.emplace_back(make_unique<DevicePage>(dev_path));
    auto& dev_page = devices.back();
    dev_page->root().show_all();

    //auto title = dev_page->name();
    auto name = device.name().value();
    device_notebook->append_page(dev_page->root(), name);
}


void
App::remove_device(const UDevice& device)
{
    auto dev_path = device.device_file().value();
    //cout << "removing " << dev_path << endl;
    for (size_t idx = 0; idx < devices.size(); ++idx)
        if (devices[idx]->path() == dev_path) {
            devices.erase(devices.begin() + idx);
            return;
        }
}


void
App::refresh()
{
    cout << "refresh()" << endl;

    clear_devices();

    Enumerator e{joystick_listener};

    e.match_subsystem("input");
    e.match_property("ID_INPUT_JOYSTICK", "1");
    e.match_name("event*");
    e.match_tag("uaccess");

    auto devices = e.execute();

    for (const auto& d : devices) {
        auto name = d.name();
        auto path = d.device_file();
        if (!name || !path)
            continue;
        add_device(d);
    }

}
