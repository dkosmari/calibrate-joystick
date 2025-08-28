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


#include <algorithm>
#include <cmath>
#include <valarray>

#include "axis_canvas.hpp"


using std::valarray;

using evdev::AbsInfo;


AxisCanvas::AxisCanvas(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& /* builder */,
                       const AbsInfo& orig) :
    Gtk::DrawingArea{cobject},
    orig{orig},
    calc{orig}
{}


bool
AxisCanvas::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const double width = get_allocated_width();
    const double height = get_allocated_height();
    auto style = get_style_context();

    style->render_background(cr, 0, 0, width, height);

    const double padding = 18.5;

    double orig_range = orig.max - orig.min;
    if (orig_range < 1)
        orig_range = 1;

    // transform values within [orig.min, orig.max] to [padding, width-padding]
    auto axis2canvas = [this, width, orig_range, padding](double x) -> double
    {
        return padding + std::round((x - orig.min) * double(width - 2 * padding) / orig_range);
    };

    auto fg_color = style->get_color(get_state_flags());

    cr->set_source_rgba(fg_color.get_red(),
                        fg_color.get_green(),
                        fg_color.get_blue(),
                        fg_color.get_alpha());


    {
        // draw orig min-max
        const double h = 10.5;
        const double w = 6.5;
        const double left = axis2canvas(orig.min);
        const double right = axis2canvas(orig.max);

        cr->save();

        cr->translate(0, height / 2.0);

        cr->set_line_width(3.0);

        // min
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);

        // max
        cr->move_to(right - w, -h);
        cr->line_to(right, -h);
        cr->line_to(right, +h);
        cr->line_to(right - w, +h);

        cr->stroke();

        cr->restore();
    }

    {
        // draw calc min-max
        const double r = 10.5;
        const double left = axis2canvas(calc.min);
        const double right = axis2canvas(calc.max);

        cr->save();

        cr->translate(0, height / 2.0);

        cr->set_line_width(1.0);
        cr->set_dash(valarray{2.0, 2.0}, 0);

        // calc min
        cr->arc(left + r, 0,
                r,
                M_PI/2,
                3*M_PI/2);
        cr->stroke();

        cr->arc(right - r, 0,
                r,
                3*M_PI/2,
                M_PI/2);
        cr->stroke();

        cr->restore();
    }

    {
        // draw box representing orig.flat
        const double box_height = 19;
        const double box_left = axis2canvas(-orig.flat);
        const double box_right = axis2canvas(orig.flat);
        const double box_width = box_right - box_left;
        cr->save();

        cr->set_line_width(3.0);
        cr->rectangle(box_left, (height - box_height)/2.0,
                      box_width, box_height);
        cr->stroke();

        cr->restore();
    }

    {
        // draw calc flat
        const double r = 6.5;
        const double left = axis2canvas(-calc.flat);
        const double right = axis2canvas(calc.flat);
        cr->save();

        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->set_dash(valarray{2.0, 2.0}, 0.0);
        cr->arc(left + r, 0,
                r,
                M_PI/2, 3*M_PI/2);
        cr->arc(right - r, 0,
                r,
                3*M_PI/2, M_PI/2);
        cr->close_path();
        cr->stroke();

        cr->restore();
    }

    {
        // draw value marker
        const double r = 2.5;
        cr->save();

        cr->translate(axis2canvas(calc.val), height / 2.0);

        cr->move_to(+r,  0);
        cr->line_to( 0, -r);
        cr->line_to(-r,  0);
        cr->line_to( 0, +r);
        cr->close_path();
        cr->fill();

        const double fs = 3.5;
        {
            // "<>" markers for orig fuzz
            const double ow = axis2canvas(orig.fuzz) - axis2canvas(0);
            const double oh = std::min<double>(fs, ow);
            // left
            cr->set_line_width(1.0);
            cr->move_to(-ow + oh, -oh);
            cr->line_to(-ow, 0);
            cr->line_to(-ow + oh, +oh);
            // right
            cr->move_to(ow - oh, -oh);
            cr->line_to(ow, 0);
            cr->line_to(ow - oh, +oh);
            cr->stroke();
        }

        {
            // "<>" markers for calc fuzz
            const double cw = axis2canvas(calc.fuzz) - axis2canvas(0);
            const double ch = std::min<double>(fs, cw);
            // left
            cr->set_line_width(1.0);
            cr->set_dash(valarray{1.0, 1.0}, 0.5);
            cr->move_to(-cw + ch, -ch);
            cr->line_to(-cw, 0);
            cr->line_to(-cw + ch, +ch);
            // right
            cr->move_to(cw - ch, -ch);
            cr->line_to(cw, 0);
            cr->line_to(cw - ch, +ch);
            cr->stroke();
        }

        cr->restore();
    }
    return true;
}


void
AxisCanvas::reset(const AbsInfo& new_orig,
                  const AbsInfo& new_calc)
{
    orig = new_orig;
    update(new_calc);
}


void
AxisCanvas::update(const AbsInfo& new_calc)
{
    calc = new_calc;
    queue_draw();
}
