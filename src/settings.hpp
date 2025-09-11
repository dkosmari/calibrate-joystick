/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gtkmm.h>

#include <libevdevxx/AbsInfo.hpp>


class AxisCanvas;

class Settings : public Gtk::ApplicationWindow {

    Gtk::ColorButton* background_color_button = nullptr;
    Gtk::ColorButton* value_color_button      = nullptr;
    Gtk::ColorButton* min_color_button        = nullptr;
    Gtk::ColorButton* max_color_button        = nullptr;
    Gtk::ColorButton* fuzz_color_button       = nullptr;
    Gtk::ColorButton* flat_color_button       = nullptr;

    AxisCanvas* sample_axis_canvas = nullptr;
    evdev::AbsInfo sample_info_orig;
    evdev::AbsInfo sample_info_calc;
    double sample_time = 0;
    sigc::connection sample_timeout_handle;

    Glib::RefPtr<Gio::Settings> settings;


    void
    on_show()
        override;

    void
    on_hide()
        override;


    bool
    animate_axis_sample();

public:

    Settings(BaseObjectType* cobject,
             const Glib::RefPtr<Gtk::Builder>& builder);


    void
    on_action_close();

    void
    on_action_reset();

}; // class Settings

#endif
