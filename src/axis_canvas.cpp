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
    const double padding = 18;

    const double width = get_allocated_width();
    const double height = get_allocated_height();

    auto style = get_style_context();


    style->render_background(cr, 0, 0, width, height);


    double range = orig.max - orig.min;
    if (range < 1)
        range = 1;

    auto translate = [this, width, range, padding](double x) -> double
    {
        return padding + (x - orig.min) * double(width - 2 * padding) / range;
    };

    auto fg_color = style->get_color(get_state_flags());

    cr->set_source_rgba(fg_color.get_red(),
                        fg_color.get_green(),
                        fg_color.get_blue(),
                        fg_color.get_alpha());
    cr->set_line_width(1.5);


    {
        // draw orig min-max
        const double h = 10.5;
        const double w = 5.5;
        const double left = translate(orig.min);
        const double right = translate(orig.max);

        cr->save();
        cr->translate(0, height / 2.0);

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
        const double h = 6.5;
        const double w = 3.5;
        const double left = translate(calc.min);
        const double right = translate(calc.max);

        cr->save();
        cr->translate(0, height / 2.0);

        // calc min
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);

        // calc max
        cr->move_to(right - w, -h);
        cr->line_to(right, -h);
        cr->line_to(right, +h);
        cr->line_to(right - w, +h);

        cr->stroke();
        cr->restore();
    }


    {
        // draw orig flat
        const double h = 9.5;
        const double left = translate(-orig.flat);
        const double w = translate(orig.flat) - left;
        cr->save();
        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->set_dash(valarray{2.0, 2.0}, 0);
        cr->rectangle(left, -h, w, 2 * h);
        cr->stroke();
        cr->restore();
    }


    {
        // draw calc flat
        const double h = 6.5;
        const double left = translate(-calc.flat);
        const double w = translate(calc.flat) - left;
        cr->save();
        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->rectangle(left, -h, w, 2 * h);
        cr->stroke();
        cr->restore();
    }


    {
        // draw value marker
        const double r = 2.5;
        cr->save();
        cr->translate(translate(calc.val), height / 2.0);

        cr->move_to(+r,  0);
        cr->line_to( 0, -r);
        cr->line_to(-r,  0);
        cr->line_to( 0, +r);
        cr->close_path();
        cr->fill();

        cr->set_line_width(1.0);

        const double fs = 3.5;
        {
            // orig fuzz
            const double ow = translate(orig.fuzz) - translate(0);
            const double oh = std::min<double>(fs, ow);
            // left
            cr->move_to(-ow + oh, -oh);
            cr->line_to(-ow, 0);
            cr->line_to(-ow + oh, +oh);
            // right
            cr->move_to(ow - oh, -oh);
            cr->line_to(ow, 0);
            cr->line_to(ow - oh, +oh);
        }


        {
            // calc fuzz
            const double cw = translate(calc.fuzz) - translate(0);
            const double ch = std::min<double>(fs, cw);
            // left
            cr->move_to(-cw + ch, -ch);
            cr->line_to(-cw, 0);
            cr->line_to(-cw + ch, +ch);
            // right
            cr->move_to(cw - ch, -ch);
            cr->line_to(cw, 0);
            cr->line_to(cw - ch, +ch);
        }


        cr->stroke();
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
