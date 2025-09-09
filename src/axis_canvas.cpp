/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>
#include <cmath>
#include <vector>

#include "axis_canvas.hpp"


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
    flat_centered{false},
    orig_fuzz_center{orig.val},
    calc_fuzz_center{orig.val}
{}


bool
AxisCanvas::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const double width = get_allocated_width();
    const double height = get_allocated_height();
    auto style = get_style_context();

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
        cr->set_dash(std::vector{2.0, 2.0}, 0);

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
        cr->set_dash(std::vector{2.0, 2.0}, 0.0);
        set_src_color(cr, flat_color);
        cr->arc(flat_left  + r, 0, r,   M_PI/2, 3*M_PI/2);
        cr->arc(flat_right - r, 0, r, 3*M_PI/2,   M_PI/2);
        cr->close_path();
        cr->stroke();
    }

    const double fuzz_size = 6.5;
    if (orig.fuzz) {
        // "<>" markers for orig fuzz
        const double half_fuzz_width = axis2canvas(orig.fuzz/2) - axis2canvas(0);
        const double half_fuzz_height = fuzz_size;
        const double fuzz_center = axis2canvas(orig_fuzz_center);
        CtxGuard guard{cr};
        set_src_color(cr, fuzz_color);
        cr->set_line_width(2.0);
        cr->translate(0, height / 2.0);
        // left
        cr->move_to(fuzz_center - half_fuzz_width, 0);
        cr->line_to(fuzz_center - half_fuzz_width + half_fuzz_height, -half_fuzz_height);
        cr->move_to(fuzz_center - half_fuzz_width, 0);
        cr->line_to(fuzz_center - half_fuzz_width + half_fuzz_height, +half_fuzz_height);
        cr->stroke();
        // right
        cr->move_to(fuzz_center + half_fuzz_width, 0);
        cr->line_to(fuzz_center + half_fuzz_width - half_fuzz_height, -half_fuzz_height);
        cr->move_to(fuzz_center + half_fuzz_width, 0);
        cr->line_to(fuzz_center + half_fuzz_width - half_fuzz_height, +half_fuzz_height);
        cr->stroke();
    }

    if (calc.fuzz) {
        // "<>" markers for calc fuzz
        const double half_fuzz_width = axis2canvas(calc.fuzz/2) - axis2canvas(0);
        const double half_fuzz_height = fuzz_size;
        const double fuzz_center = axis2canvas(calc_fuzz_center);
        CtxGuard guard{cr};
        set_src_color(cr, fuzz_color);
        cr->set_line_width(1.5);
        cr->set_dash(std::vector{1.5, 1.5}, 0.0);
        cr->translate(0, height / 2.0);
        // left
        cr->move_to(fuzz_center - half_fuzz_width, 0);
        cr->line_to(fuzz_center - half_fuzz_width + half_fuzz_height, -half_fuzz_height);
        cr->move_to(fuzz_center - half_fuzz_width, 0);
        cr->line_to(fuzz_center - half_fuzz_width + half_fuzz_height, +half_fuzz_height);
        cr->stroke();
        // right
        cr->move_to(fuzz_center + half_fuzz_width, 0);
        cr->line_to(fuzz_center + half_fuzz_width - half_fuzz_height, -half_fuzz_height);
        cr->move_to(fuzz_center + half_fuzz_width, 0);
        cr->line_to(fuzz_center + half_fuzz_width - half_fuzz_height, +half_fuzz_height);
        cr->stroke();
    }

    {
        // draw value marker as a cross
        const double marker_radius = 4.5;
        CtxGuard guard{cr};
        set_src_color(cr, value_color);
        cr->set_line_width(1.5);
        cr->translate(axis2canvas(calc.val), height / 2.0);
        cr->move_to(-marker_radius, 0);
        cr->line_to(+marker_radius, 0);
        cr->move_to(0, -marker_radius);
        cr->line_to(0, +marker_radius);
        cr->stroke();
    }

    return true;
}


void
AxisCanvas::reset(const AbsInfo& new_orig,
                  const AbsInfo& new_calc)
{
    orig = new_orig;
    update(new_calc);
    orig_fuzz_center = orig.val;
    calc_fuzz_center = orig.val;
}


namespace {

    int
    update_fuzz_center(int center,
                       int fuzz,
                       int value)
    {
        /*
         * Note: the kernel's defuzzing does this:
         *
         * Assume:
         * - real_value: the real value read from the axis this instant.
         * - user_value: the last value sent to userspace.
         * - delta = real_value - user_value
         * Then:
         * - if -fuzz/2 < delta < +fuzz/2: do not change user_value
         * - if -fuzz < delta < +fuzz: user_value = lerp(user_value, real_value, 0.25)
         * - if -2*fuzz < delta < +2*fuzz: user_value = lerp(user_value, real_value, 0.5)
         * - otherwise: user_value = real_value
         *
         * (The freedesktop folks hate this; libinput always forces the fuzz to zero, and
         * applies its own defuzzing).
         *
         * The code below corresponds to the first case, to show the region where the
         * value doesn't change at all. The real value change must be at least twice the
         * value of fuzz, in order to be free from kernel defuzzing.
         */
        int half_fuzz = fuzz / 2;
        int min = center - half_fuzz;
        int max = center + half_fuzz;
        if (value < min)
            center = value + half_fuzz;
        if (value > max)
            center = value - half_fuzz;
        return center;
    }

} // namespace


void
AxisCanvas::update(const AbsInfo& new_calc)
{
    calc = new_calc;

    // calculate fuzz centers
    int value = calc.val;

    orig_fuzz_center = update_fuzz_center(orig_fuzz_center, orig.fuzz, value);
    calc_fuzz_center = update_fuzz_center(calc_fuzz_center, calc.fuzz, value);

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
