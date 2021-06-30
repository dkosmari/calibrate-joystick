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

    template<typename T>
    using rptr = Glib::RefPtr<T>;

    template<typename T>
    using uptr = std::unique_ptr<T>;


    rptr<Gio::Resource> resource;

    uptr<Gtk::ApplicationWindow> main_window;
    uptr<Gtk::HeaderBar> header_bar;

    Gtk::Notebook* device_notebook = nullptr;

    std::map<std::filesystem::path,
             uptr<DevicePage>> devices;

    uptr<gudev::Client> uclient;


    void on_startup() override;

    void on_activate() override;

    void on_open(const type_vec_files& files,
                 const Glib::ustring& hint) override;

    int on_handle_local_options(const rptr<Glib::VariantDict>& options);

    void on_action_about();

    void on_action_open();

    void on_action_refresh();

    bool load_resources(const std::filesystem::path& res_path);

    void present_gui();

    void on_uevent(const std::string& action, const gudev::Device& device);

public:

    App();

    ~App();


    void clear_devices();

    void add_device(const std::filesystem::path& dev_path);

    void remove_device(const std::filesystem::path& dev_path);



};


#endif
