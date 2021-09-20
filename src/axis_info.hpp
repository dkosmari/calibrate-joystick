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


#ifndef AXIS_INFO_HPP
#define AXIS_INFO_HPP


#include <memory>
#include <string>

#include <gtkmm.h>
#include <libevdevxx/abs_info.hpp>
#include <libevdevxx/event.hpp>


class AxisCanvas;


class AxisInfo {

    template<typename T>
    using uptr = std::unique_ptr<T>;

    template<typename T>
    using rptr = Glib::RefPtr<T>;


    rptr<Gio::SimpleActionGroup> actions;
    rptr<Gio::SimpleAction> apply_action;
    rptr<Gio::SimpleAction> reset_action;


    using LabelPtr = uptr<Gtk::Label>;
    using SpinButtonPtr = uptr<Gtk::SpinButton>;


    uptr<Gtk::Frame> info_frame;
    LabelPtr name_label;

    LabelPtr value_label;
    LabelPtr orig_min_label;
    LabelPtr orig_max_label;
    LabelPtr orig_fuzz_label;
    LabelPtr orig_flat_label;
    LabelPtr orig_res_label;

    SpinButtonPtr calc_min_spin;
    SpinButtonPtr calc_max_spin;
    SpinButtonPtr calc_fuzz_spin;
    SpinButtonPtr calc_flat_spin;
    SpinButtonPtr calc_res_spin;

    AxisCanvas* axis_canvas;

    sigc::connection min_changed_conn;
    sigc::connection max_changed_conn;
    sigc::connection fuzz_changed_conn;
    sigc::connection flat_changed_conn;
    sigc::connection res_changed_conn;

    evdev::Event::Code code;
    evdev::AbsInfo orig;
    evdev::AbsInfo calc;



    void load_widgets();

    void update_canvas();

    void update_min(int min);
    void update_max(int max);
    void update_fuzz(int fuzz);
    void update_flat(int flat);
    void update_res(int res);

    void on_action_apply();
    void on_action_reset();

public:

    AxisInfo(evdev::Event::Code axis_code,
             const evdev::AbsInfo& info);

    // disable moving
    AxisInfo(AxisInfo&& other) = delete;

    void update_value(int value);

    Gtk::Widget& root();

    const evdev::AbsInfo& get_calc() const noexcept;

    void reset(const evdev::AbsInfo& new_orig);

    void disable();
};


#endif
