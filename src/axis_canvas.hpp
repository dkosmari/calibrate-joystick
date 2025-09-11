/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AXIS_CANVAS_HPP
#define AXIS_CANVAS_HPP

#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include <libevdevxx/AbsInfo.hpp>

#include "colors.hpp"


class AxisCanvas : public Gtk::DrawingArea {

    evdev::AbsInfo orig;
    evdev::AbsInfo calc;
    bool flat_centered;
    int orig_fuzz_center;
    int calc_fuzz_center;

    Colors colors;


    bool
    on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
        override;

public:

    AxisCanvas(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& /* builder */,
               const evdev::AbsInfo& orig);


    void
    reset(const evdev::AbsInfo& new_orig,
          const evdev::AbsInfo& new_calc);

    void
    update(const evdev::AbsInfo& new_calc);


    void
    set_flat_centered(bool is_centered);


    void
    set_colors(const Colors& c);


    void
    set_background_color(const Gdk::RGBA& c);

    void
    set_value_color(const Gdk::RGBA& c);

    void
    set_min_color(const Gdk::RGBA& c);

    void
    set_max_color(const Gdk::RGBA& c);

    void
    set_fuzz_color(const Gdk::RGBA& c);

    void
    set_flat_color(const Gdk::RGBA& c);

}; // class AxisCanvas

#endif
