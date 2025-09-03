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

#include "app.hpp"


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


    void
    set_src_color(const Cairo::RefPtr<Cairo::Context>& ctx,
                  const Gdk::RGBA& color)
    {
        ctx->set_source_rgba(color.get_red(),
                             color.get_green(),
                             color.get_blue(),
                             color.get_alpha());
    }

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

    auto app = Glib::RefPtr<App>::cast_static(App::get_default());

    set_src_color(cr, background_color);
    cr->rectangle(0, 0, width, height);
    cr->fill();

    const double padding = 18.5;

    double orig_range = orig.max - orig.min;
    if (orig_range < 1)
        orig_range = 1;

    // transform values within [orig.min, orig.max] to [padding, width-padding]
    auto axis2canvas = [this, width, orig_range, padding](double x) -> double
    {
        return padding + std::round((x - orig.min) * double(width - 2 * padding) / orig_range);
    };

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
        set_src_color(cr, min_color);
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);
        cr->stroke();

        // max
        set_src_color(cr, max_color);
        cr->move_to(right - w, -h);
        cr->line_to(right, -h);
        cr->line_to(right, +h);
        cr->line_to(right - w, +h);
        cr->stroke();

    }

    {
        // draw calc min-max
        const double r = 9.5;
        const double left = axis2canvas(calc.min);
        const double right = axis2canvas(calc.max);

        CtxGuard guard{cr};

        cr->translate(0, height / 2.0);

        cr->set_line_width(1.5);
        cr->set_dash(valarray{2.0, 2.0}, 0);

        // calc min
        set_src_color(cr, min_color);
        cr->arc(left + r, 0, r, M_PI/2, 3*M_PI/2);
        cr->stroke();

        // calc max
        set_src_color(cr, max_color);
        cr->arc(right - r, 0, r, 3*M_PI/2, M_PI/2);
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
        set_src_color(cr, flat_color);
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
        set_src_color(cr, flat_color);
        cr->arc(flat_left  + r, 0, r,   M_PI/2, 3*M_PI/2);
        cr->arc(flat_right - r, 0, r, 3*M_PI/2,   M_PI/2);
        cr->close_path();
        cr->stroke();
    }

    {
        // draw value marker
        const double r = 3.5;

        CtxGuard guard{cr};

        cr->translate(axis2canvas(calc.val), height / 2.0);

        set_src_color(cr, value_color);
        cr->move_to(+r,  0);
        cr->line_to( 0, -r);
        cr->line_to(-r,  0);
        cr->line_to( 0, +r);
        cr->close_path();
        cr->fill();

        // draw fuzz
        set_src_color(cr, fuzz_color);
        const double fuzz_size = 5.5;
        {
            // "<>" markers for orig fuzz
            const double fuzz_width = axis2canvas(orig.fuzz) - axis2canvas(0);
            const double fuzz_height = std::min<double>(fuzz_size, fuzz_width);
            // left
            cr->set_line_width(2.0);
            cr->move_to(-fuzz_width + fuzz_height, -fuzz_height);
            cr->line_to(-fuzz_width, 0);
            cr->line_to(-fuzz_width + fuzz_height, +fuzz_height);
            // right
            cr->move_to(fuzz_width - fuzz_height,  -fuzz_height);
            cr->line_to(fuzz_width, 0);
            cr->line_to(fuzz_width - fuzz_height,  +fuzz_height);
            cr->stroke();
        }

        {
            // "<>" markers for calc fuzz
            const double fuzz_width = axis2canvas(calc.fuzz) - axis2canvas(0);
            const double fuzz_height = std::min<double>(fuzz_size, fuzz_width);
            // left
            cr->set_line_width(1.5);
            cr->set_dash(valarray{1.0, 1.0}, 0.5);
            cr->move_to(-fuzz_width + fuzz_height, -fuzz_height);
            cr->line_to(-fuzz_width, 0);
            cr->line_to(-fuzz_width + fuzz_height, +fuzz_height);
            // right
            cr->move_to(fuzz_width - fuzz_height,  -fuzz_height);
            cr->line_to(fuzz_width, 0);
            cr->line_to(fuzz_width - fuzz_height,  +fuzz_height);
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


void
AxisCanvas::set_background_color(const Gdk::RGBA& color)
{
    background_color = color;
}


void
AxisCanvas::set_value_color(const Gdk::RGBA& color)
{
    value_color = color;
}


void
AxisCanvas::set_min_color(const Gdk::RGBA& color)
{
    min_color = color;
}


void
AxisCanvas::set_max_color(const Gdk::RGBA& color)
{
    max_color = color;
}


void
AxisCanvas::set_fuzz_color(const Gdk::RGBA& color)
{
    fuzz_color = color;
}


void
AxisCanvas::set_flat_color(const Gdk::RGBA& color)
{
    flat_color = color;
}
