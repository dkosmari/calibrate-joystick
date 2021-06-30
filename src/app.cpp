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
#include <vector>

#include <gudevxx/enumerator.hpp>

#include "app.hpp"

#include "device_page.hpp"
#include "utils.hpp"


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glibmm/i18n.h>


using std::cout;
using std::endl;
using std::filesystem::path;
using std::make_unique;
using std::string;
using std::vector;

using Gio::ApplicationFlags;
using Gio::Resource;
using Glib::RefPtr;
using Glib::ustring;
using Glib::Variant;
using Glib::VariantBase;
using Glib::VariantType;


static const auto app_flags =
    ApplicationFlags::APPLICATION_HANDLES_OPEN;



App::App() :
    Gtk::Application{"none.calibrate_joystick", app_flags}
{
    signal_handle_local_options()
        .connect(sigc::mem_fun(this, &App::on_handle_local_options));
    add_main_option_entry(OptionType::OPTION_TYPE_BOOL,
                          "version", 'V',
                          _("Show program version"));

    if (!load_resources("resources.gresource") &&
        !load_resources(RESOURCE_DIR "/resources.gresource"))
        throw std::runtime_error{_("Could not load resource file.")};
}


App::~App() = default;


void
App::on_startup()
{
    cout << __PRETTY_FUNCTION__ << endl;

    Gtk::Application::on_startup();

    add_action("about",   sigc::mem_fun(this, &App::on_action_about));
    add_action("open",    sigc::mem_fun(this, &App::on_action_open));
    add_action("quit",    sigc::mem_fun(this, &App::quit));
    add_action("refresh", sigc::mem_fun(this, &App::on_action_refresh));
}


void
App::on_activate()
{
    cout << __PRETTY_FUNCTION__ << endl;

    present_gui();

    activate_action("refresh");
}


void
App::on_open(const type_vec_files& files,
             const ustring&)
{
    cout << __PRETTY_FUNCTION__ << endl;

    present_gui();
    clear_devices();
    uclient.reset();

    for (auto& f : files) {
        path dev_path = f->get_path();
        cout << "  " << dev_path << endl;
        add_device(dev_path);
    }
}


int
App::on_handle_local_options(const RefPtr<Glib::VariantDict>& options)
{
    if (options->contains("version")) {
        cout << PACKAGE_VERSION << endl;
        return 0;
    }
    return -1;
}


void
App::on_action_about()
{
    auto builder = Gtk::Builder::create_from_resource(ui_about_dialog_path);
    auto diag = get_widget<Gtk::AboutDialog>(builder, "about_dialog");
    diag->set_application(main_window->get_application());
    diag->set_version(PACKAGE_VERSION);
    diag->add_button(_("_Close"), Gtk::ResponseType::RESPONSE_CLOSE);
    diag->run();
}


void
App::on_action_open()
{
    cout << __PRETTY_FUNCTION__ << endl;

    Gtk::FileChooserDialog diag{*main_window, _("Open device...")};
    diag.set_select_multiple();

    diag.set_current_folder("/dev/input");
    diag.add_shortcut_folder("/dev/input");

    diag.add_button(_("_Cancel"), Gtk::ResponseType::RESPONSE_CANCEL);
    diag.add_button(_("_Open"), Gtk::ResponseType::RESPONSE_ACCEPT);

    auto evdev_filter = Gtk::FileFilter::create();
    evdev_filter->set_name(_("evdev devices"));
    evdev_filter->add_pattern("event*");
    diag.add_filter(evdev_filter);

    auto all_filter = Gtk::FileFilter::create();
    all_filter->set_name(_("all files"));
    all_filter->add_pattern("*");
    diag.add_filter(all_filter);


    if (diag.run() == Gtk::ResponseType::RESPONSE_ACCEPT)
        open(diag.get_files());
}


bool
App::load_resources(const std::filesystem::path& res_path)
{
    try {
        g_debug("Trying to load resources from \"%s\".", res_path.c_str());
        resource = Resource::create_from_file(res_path);
        if (!resource)
            return false;
        g_debug("Loaded resource file from \"%s\".", res_path.c_str());
        resource->register_global();
        return true;
    }
    catch (...) {
        return false;
    }
}


void
App::present_gui()
{
    if (!main_window) {
        auto builder = Gtk::Builder::create_from_resource(ui_main_window_path);

        main_window = get_widget<Gtk::ApplicationWindow>(builder,
                                                         "main_window");
        add_window(*main_window);

        header_bar = get_widget<Gtk::HeaderBar>(builder,
                                                "header_bar");
        main_window->set_titlebar(*header_bar);

        builder->get_widget("device_notebook", device_notebook);
    }

    main_window->present();
}


void
App::clear_devices()
{
    devices.clear();
}


void
App::add_device(const path& dev_path)
{
    try {
        auto [iter, inserted] = devices.emplace(dev_path,
                                            make_unique<DevicePage>(dev_path));
        if (!inserted)
            return;

        auto& page = iter->second;
        device_notebook->append_page(page->root(), page->name());
    }
    catch (std::exception& e) {
        Gtk::MessageDialog dialog{
            *main_window,
            _("Error opening file"),
            false,
            Gtk::MessageType::MESSAGE_ERROR};
        dialog.set_secondary_text(e.what());
        dialog.run();
    }
}


void
App::remove_device(const path& dev_path)
{
    devices.erase(dev_path);
}


void
App::on_action_refresh()
{
    cout << __PRETTY_FUNCTION__ << endl;

    clear_devices();

    if (!uclient) {
        uclient = make_unique<gudev::Client>(vector<string>{"input"});
        uclient->uevent_callback =
            [this](const string& action,
                   const gudev::Device& device)
            {
                on_uevent(action, device);
            };
    }

    gudev::Enumerator e{*uclient};

    e.match_subsystem("input");
    e.match_property("ID_INPUT_JOYSTICK", "1");
    e.match_name("event*");

    auto devices = e.execute();

    for (const auto& d : devices) {
        auto name = d.name();
        auto path = d.device_file();
        if (!name || !path)
            continue;
        add_device(*path);
    }
}


void
App::on_uevent(const string& action,
               const gudev::Device& device)
{
    if (auto name = device.name();
        !name || !starts_with(*name, "event"))
        return;

    if (!device.property_as<bool>("ID_INPUT_JOYSTICK"))
        return;

    auto dev_path = device.device_file();
    if (!dev_path)
        return;

    if (action == "add")
        add_device(*dev_path);
    else if (action == "remove")
        remove_device(*dev_path);
}
