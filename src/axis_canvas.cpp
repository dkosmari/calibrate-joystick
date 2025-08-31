/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>
#include <cmath>
#include <valarray>

#include "axis_canvas.hpp"


using std::valarray;

using evdev::AbsInfo;


namespace {

    // RAII class to call save/restore on the context.
    struct CtxGuard {

        const Cairo::RefPtr<Cairo::Context>& ctx;

        CtxGuard(const Cairo::RefPtr<Cairo::Context>& ctx_) :
            ctx(ctx_)
        {
            ctx->save();
        }

        ~CtxGuard()
            noexcept
        {
            ctx->restore();
        }

    };

} // namespace


AxisCanvas::AxisCanvas(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& /* builder */,
                       const AbsInfo& orig) :
    Gtk::DrawingArea{cobject},
    orig{orig},
    calc{orig},
    flat_centered{false}
{}


bool
AxisCanvas::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const double width = get_allocated_width();
    const double height = get_allocated_height();
    auto style = get_style_context();

#if 1
    style->render_background(cr, 0, 0, width, height);
#else
    // TODO: load a color palette.
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->rectangle(0, 0, width, height);
    cr->fill();
#endif

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

        CtxGuard guard{cr};

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

    }

    {
        // draw calc min-max
        const double r = 10.5;
        const double left = axis2canvas(calc.min);
        const double right = axis2canvas(calc.max);

        CtxGuard guard{cr};

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
    }

    {
        // draw box representing orig.flat
        const double flat_height = 19;
        const double flat_anchor = flat_centered ? (orig.min + orig.max) / 2.0 : 0.0;
        const double flat_left   = axis2canvas(flat_anchor - orig.flat);
        const double flat_right  = axis2canvas(flat_anchor + orig.flat);
        const double flat_width  = flat_right - flat_left;

        CtxGuard guard{cr};

        cr->set_line_width(3.0);
        cr->rectangle(flat_left, (height - flat_height)/2.0,
                      flat_width, flat_height);
        cr->stroke();
    }

    {
        // draw calc flat
        const double r = 6.5;
        const double flat_anchor = flat_centered ? (calc.min + calc.max) / 2.0 : 0.0;
        const double flat_left   = axis2canvas(flat_anchor - calc.flat);
        const double flat_right  = axis2canvas(flat_anchor + calc.flat);

        CtxGuard guard{cr};

        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->set_dash(valarray{2.0, 2.0}, 0.0);
        cr->arc(flat_left  + r, 0, r,   M_PI/2, 3*M_PI/2);
        cr->arc(flat_right - r, 0, r, 3*M_PI/2,   M_PI/2);
        cr->close_path();
        cr->stroke();
    }

    {
        // draw value marker
        const double r = 2.5;

        CtxGuard guard{cr};

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


void
AxisCanvas::set_flat_centered(bool is_centered)
{
    flat_centered = is_centered;
    queue_draw();
}
