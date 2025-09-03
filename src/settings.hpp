/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gtkmm.h>

class App;

class Settings : public Gtk::ApplicationWindow {

    App* app;

    Gtk::ColorButton* background_color_button = nullptr;
    Gtk::ColorButton* value_color_button      = nullptr;
    Gtk::ColorButton* min_color_button        = nullptr;
    Gtk::ColorButton* max_color_button        = nullptr;
    Gtk::ColorButton* fuzz_color_button       = nullptr;
    Gtk::ColorButton* flat_color_button       = nullptr;

    Glib::RefPtr<Gio::Settings> settings;


public:

    Settings(BaseObjectType* cobject,
             const Glib::RefPtr<Gtk::Builder>& builder,
             App* app_);


    void
    on_action_close();

    void
    on_action_reset();

}; // class Settings

#endif
