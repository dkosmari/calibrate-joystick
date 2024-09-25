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


#ifndef AXIS_CANVAS_HPP
#define AXIS_CANVAS_HPP

#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include <libevdevxx/AbsInfo.hpp>


class AxisCanvas : public Gtk::DrawingArea {

    evdev::AbsInfo orig;
    evdev::AbsInfo calc;


    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

public:

    AxisCanvas(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& /* builder */,
               const evdev::AbsInfo& orig);


    void reset(const evdev::AbsInfo& new_orig, const evdev::AbsInfo& new_calc);
    void update(const evdev::AbsInfo& new_calc);
};


#endif
