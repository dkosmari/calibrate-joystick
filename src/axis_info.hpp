/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2021  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AXIS_INFO_HPP
#define AXIS_INFO_HPP

#include <memory>
#include <string>

#include <gtkmm.h>
#include <libevdevxx/AbsInfo.hpp>
#include <libevdevxx/Event.hpp>

#include "colors.hpp"


class AxisCanvas;


class AxisInfo {

    Glib::RefPtr<Gio::SimpleActionGroup> actions;
    Glib::RefPtr<Gio::SimpleAction> action_apply;
    Glib::RefPtr<Gio::SimpleAction> action_revert;
    Glib::RefPtr<Gio::SimpleAction> action_flat_zero;
    Glib::RefPtr<Gio::SimpleAction> action_flat_centered;

    std::unique_ptr<Gtk::Frame> info_frame;

    Gtk::Label* name_label = nullptr;

    Gtk::Label* value_label     = nullptr;
    Gtk::Label* orig_min_label  = nullptr;
    Gtk::Label* orig_max_label  = nullptr;
    Gtk::Label* orig_fuzz_label = nullptr;
    Gtk::Label* orig_flat_label = nullptr;
    Gtk::Label* orig_res_label  = nullptr;

    Gtk::SpinButton* calc_min_spin  = nullptr;
    Gtk::SpinButton* calc_max_spin  = nullptr;
    Gtk::SpinButton* calc_fuzz_spin = nullptr;
    Gtk::SpinButton* calc_flat_spin = nullptr;
    Gtk::SpinButton* calc_res_spin  = nullptr;

    Gtk::RadioMenuItem* flat_item_zero     = nullptr;
    Gtk::RadioMenuItem* flat_item_centered = nullptr;

    AxisCanvas* axis_canvas = nullptr;

    evdev::Code code;
    evdev::AbsInfo orig;
    evdev::AbsInfo calc;

    bool flat_centered = false;


    void
    create_actions();

    void
    load_widgets();

    void
    update_canvas();

    void
    set_calc_min(int min);

    void
    set_calc_max(int max);

    void
    set_calc_fuzz(int fuzz);

    void
    set_calc_flat(int flat);

    void
    set_calc_res(int res);


    void
    on_action_apply();

    void
    on_action_revert();


    void
    on_changed_flat_to_zero();

    void
    on_changed_flat_to_centered();

    // Forbid moving.
    AxisInfo(AxisInfo&& other) = delete;

public:

    AxisInfo(evdev::Code axis_code,
             const evdev::AbsInfo& info);

    void
    set_calc_value(int value);

    Gtk::Widget&
    root();

    const evdev::AbsInfo&
    get_calc()
        const noexcept;

    void
    reset(const evdev::AbsInfo& new_orig);

    void
    disable();


    void
    set_colors(const Colors&);


    void
    set_flat_centered(bool centered);

    bool
    is_flat_centered()
        const;

}; // class AxisInfo

#endif
