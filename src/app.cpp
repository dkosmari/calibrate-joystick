#include <iostream>

#include <gudevxx/enumerator.hpp>

#include "app.hpp"

#include "device_page.hpp"
#include "utils.hpp"


using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::make_unique;

using UDevice = gudev::Device;
using gudev::Enumerator;


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

    builder->get_widget_derived("refresh_button", refresh_button, *this);

    builder->get_widget("device_stack", device_stack);

    add_window(*main_window);
}


void
App::on_activate()
{
    create_main_window();
    main_window->show_all();
    main_window->present();

    refresh_joysticks();
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

    auto title = dev_page->name();
    auto name = device.name().value();
    device_stack->add(dev_page->root(), name, title);

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
App::refresh_joysticks()
{
    //cout << "refresh" << endl;
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
