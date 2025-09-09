/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iostream>
#include <vector>

#include <glib.h>
#ifdef G_OS_UNIX
#include <glib-unix.h>
#endif

#include <gudevxx/Enumerator.hpp>

#include "app.hpp"

#include "controller_db.hpp"
#include "device_page.hpp"
#include "utils.hpp"
#include "settings.hpp"


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




#if 0
#define TRACE cout << __PRETTY_FUNCTION__ << endl
#else
#define TRACE while (false)
#endif


namespace {

    const auto app_flags = ApplicationFlags::APPLICATION_HANDLES_OPEN;

    const std::string application_glade = RESOURCE_PREFIX "/ui/application.glade";

} // namespace


#ifdef G_OS_UNIX
static
gboolean
stop_application(App* app)
{
    app->quit();
    return FALSE;
}
#endif


App::App() :
    Gtk::Application{APPLICATION_ID, app_flags}
{
    ControllerDB::initialize();

    signal_handle_local_options()
        .connect(sigc::mem_fun(this, &App::on_handle_local_options));

    add_main_option_entry(OptionType::OPTION_TYPE_BOOL,
                          "version", 'V',
                          _("Show program version."));

    add_main_option_entry(OptionType::OPTION_TYPE_BOOL,
                          "daemon", 'd',
                          _("Run in daemon mode."));

    if (!load_resources(PACKAGE ".gresource") &&
        !load_resources(RESOURCES_DIR "/" PACKAGE ".gresource"))
        throw std::runtime_error{_("Could not load resources file.")};
}


App::~App()
{
    ControllerDB::finalize();
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
App::load_gui()
{
    if (main_window)
        return;

    auto builder = Gtk::Builder::create_from_resource(application_glade);

    main_window = get_widget<Gtk::ApplicationWindow>(builder, "main_window");

    header_bar = get_widget<Gtk::HeaderBar>(builder, "header_bar");
    main_window->set_titlebar(*header_bar);

    builder->get_widget("device_notebook", device_notebook);

    builder->get_widget("quit_button", quit_button);
    if (opt_daemon) {
        quit_button->show();

        // every time the window is hidden, send a new notification
        main_window->signal_delete_event().connect([this](GdkEventAny*) -> bool
        {
            if (!status_icon->is_embedded())
                send_daemon_notification();
            return false;
        });
    }

    about_window = get_widget<Gtk::AboutDialog>(builder, "about_dialog");
    about_window->set_program_name(PACKAGE_NAME);
    about_window->set_website(PACKAGE_URL);
    about_window->set_version(PACKAGE_VERSION);
    about_window->add_button(_("_Close"), Gtk::ResponseType::RESPONSE_CLOSE);

    settings_window = get_widget_derived<Settings>(builder, "settings_window", this);
}


void
App::present_main_window()
{
    TRACE;

    add_window(*main_window);
    main_window->present();
}


void
App::connect_uevent()
{
    if (!uclient) {
        uclient = gudev::Client{vector<string>{"input"}};
        uclient.uevent_callback =
            [this](const string& action,
                   const gudev::Device& device)
            {
                on_uevent(action, device);
            };
    }
}


void
App::send_daemon_notification()
{
    auto notif = Gio::Notification::create(_("Daemon running."));
    notif->set_body(_("Monitoring new joysticks..."));
    notif->set_priority(Gio::NotificationPriority::NOTIFICATION_PRIORITY_LOW);
    notif->add_button(_("Quit daemon"), "app.quit");
    send_notification("daemon", notif);
}


void
App::on_action_about()
{
    add_window(*about_window);
    about_window->run();
    about_window->hide();
}


void
App::on_action_open()
{
    TRACE;

    Gtk::FileChooserDialog diag{*main_window, _("Open device...")};
    add_window(diag);
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


void
App::on_action_quit()
{
    TRACE;

    if (opt_daemon) {
        withdraw_notification("daemon");
        release();
    }
    quit();
}


void
App::on_action_refresh()
{
    TRACE;

    clear_devices();

    connect_uevent();

    gudev::Enumerator e{uclient};

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
App::on_action_settings()
{
    add_window(*settings_window);
    settings_window->present();
}


void
App::on_activate()
{
    TRACE;

    load_gui();

    if (opt_daemon && silent_start) {
        silent_start = false;
        return;
    }

    present_main_window();
    // activate_action("refresh");
}


int
App::on_handle_local_options(const RefPtr<Glib::VariantDict>& options)
{
    TRACE;

    if (options->contains("version")) {
        cout << PACKAGE_VERSION << endl;
        return 0;
    }

    if (options->contains("daemon")) {
        silent_start = true;
        opt_daemon = true;
    }

    return -1;
}


void
App::on_open(const type_vec_files& files,
             const ustring&)
{
    TRACE;

    silent_start = false;

    present_main_window();
    clear_devices();

    // if not running as daemon, stop auto-refreshing
    if (!opt_daemon)
        uclient.destroy();

    for (auto& f : files) {
        path dev_path = f->get_path();
        add_device(dev_path);
    }
}


void
App::on_startup()
{
    TRACE;

    Gtk::Application::on_startup();

#ifdef G_OS_UNIX
    g_unix_signal_add(SIGINT, G_SOURCE_FUNC(stop_application), this);
    g_unix_signal_add(SIGTERM, G_SOURCE_FUNC(stop_application), this);
#endif

    add_action("about",    sigc::mem_fun(this, &App::on_action_about));
    add_action("open",     sigc::mem_fun(this, &App::on_action_open));
    add_action("quit",     sigc::mem_fun(this, &App::on_action_quit));
    add_action("refresh",  sigc::mem_fun(this, &App::on_action_refresh));
    add_action("settings", sigc::mem_fun(this, &App::on_action_settings));

    if (opt_daemon) {
        //cout << "running as daemon" << endl;
        hold(); // keep it running without window
        connect_uevent();

        status_icon = Gtk::StatusIcon::create("input-gaming");
        status_icon->set_name("none.calibrate_joystick");
        status_icon->set_title(_("Calibrate Joystick"));
        status_icon->set_tooltip_text(_("Monitoring new joysticks..."));
        status_icon->signal_activate().connect(sigc::mem_fun(this, &App::activate));
        status_icon->set_visible(true);

        // if no status icon is available, send a notification instead
        if (!status_icon->is_embedded())
            send_daemon_notification();
    }
}


void
App::on_uevent(const string& action,
               const gudev::Device& device)
{
    TRACE;

    if (auto name = device.name();
        !name || !name->starts_with("event"))
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


void
App::update_colors()
{
    for (auto& [key, val] : devices)
        val->update_colors(this);
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
        auto [iter, inserted] =
            devices.emplace(dev_path, make_unique<DevicePage>(dev_path));
        if (!inserted)
            return;

        auto& page = iter->second;
        device_notebook->append_page(page->root(), page->name());
        page->update_colors(this);
    }
    catch (std::exception& e) {
        present_main_window();
        Gtk::MessageDialog dialog{
            *main_window,
            _("Error opening file"),
            false,
            Gtk::MessageType::MESSAGE_ERROR};
        dialog.set_secondary_text(e.what());
        dialog.run();
        return;
    }

    // Only pop up the main window if there's no config for this device.
    if (!devices.at(dev_path)->has_loaded_config())
        present_main_window();
}


void
App::remove_device(const path& dev_path)
{
    devices.erase(dev_path);
}


const Gdk::RGBA&
App::get_background_color()
    const noexcept
{
    return background_color;
}


const Gdk::RGBA&
App::get_value_color()
    const noexcept
{
    return value_color;
}


const Gdk::RGBA&
App::get_min_color()
    const noexcept
{
    return min_color;
}


const Gdk::RGBA&
App::get_max_color()
    const noexcept
{
    return max_color;
}


const Gdk::RGBA&
App::get_fuzz_color()
    const noexcept
{
    return fuzz_color;
}


const Gdk::RGBA&
App::get_flat_color()
    const noexcept
{
    return flat_color;
}


void
App::set_background_color(const Gdk::RGBA& color)
{
    background_color = color;
    update_colors();
}


void
App::set_value_color(const Gdk::RGBA& color)
{
    value_color = color;
    update_colors();
}


void
App::set_min_color(const Gdk::RGBA& color)
{
    min_color = color;
    update_colors();
}


void
App::set_max_color(const Gdk::RGBA& color)
{
    max_color = color;
    update_colors();
}


void
App::set_fuzz_color(const Gdk::RGBA& color)
{
    fuzz_color = color;
    update_colors();
}


void
App::set_flat_color(const Gdk::RGBA& color)
{
    flat_color = color;
    update_colors();
}
