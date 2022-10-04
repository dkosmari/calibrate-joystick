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
#include <filesystem>
#include <map>
#include <memory>
#include <string>

// system libraries
#include <gtkmm.h>

#include <gudevxx/client.hpp>


class DevicePage;


class App : public Gtk::Application {

    Glib::RefPtr<Gio::Resource> resource;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;

    Gtk::Notebook* device_notebook = nullptr;
    Gtk::Button* quit_button = nullptr;

    std::map<std::filesystem::path,
             std::unique_ptr<DevicePage>> devices;

    std::unique_ptr<gudev::Client> uclient;

    Glib::RefPtr<Gtk::StatusIcon> status_icon;

    bool opt_daemon = false;
    bool silent_start = false;


    bool load_resources(const std::filesystem::path& res_path);
    void present_gui();
    void connect_uevent();
    void send_daemon_notification();

    void on_action_about();
    void on_action_open();
    void on_action_quit();
    void on_action_refresh();
    void on_activate() override;
    int  on_handle_local_options(const Glib::RefPtr<Glib::VariantDict>& options);
    void on_open(const type_vec_files& files,
                 const Glib::ustring& hint) override;
    void on_startup() override;
    void on_uevent(const std::string& action, const gudev::Device& device);

public:

    App();

    ~App();


    void clear_devices();

    void add_device(const std::filesystem::path& dev_path);

    void remove_device(const std::filesystem::path& dev_path);

};


#endif
