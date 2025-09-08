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


class App;
class AxisCanvas;


class AxisInfo {

    template<typename T>
    using rptr = Glib::RefPtr<T>;

    rptr<Gio::SimpleActionGroup> actions;
    rptr<Gio::SimpleAction> action_apply;
    rptr<Gio::SimpleAction> action_revert;
    rptr<Gio::SimpleAction> action_flat_zero;
    rptr<Gio::SimpleAction> action_flat_centered;

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

    sigc::connection min_changed_conn;
    sigc::connection max_changed_conn;
    sigc::connection fuzz_changed_conn;
    sigc::connection flat_changed_conn;
    sigc::connection res_changed_conn;

    evdev::Code code;
    evdev::AbsInfo orig;
    evdev::AbsInfo calc;

    bool flat_centered = false;


    void
    load_widgets();

    void
    update_canvas();

    void
    update_min(int min);

    void
    update_max(int max);

    void
    update_fuzz(int fuzz);

    void
    update_flat(int flat);

    void
    update_res(int res);


    void
    on_action_apply();

    void
    on_action_revert();


    void
    on_changed_flat_to_zero();

    void
    on_changed_flat_to_centered();

    // Disallow moving.
    AxisInfo(AxisInfo&& other) = delete;

public:

    AxisInfo(evdev::Code axis_code,
             const evdev::AbsInfo& info);

    void
    update_value(int value);

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
    update_colors(const App* app);


    void
    set_flat_centered(bool centered);

    bool
    is_flat_centered()
        const;

}; // class AxisInfo

#endif
