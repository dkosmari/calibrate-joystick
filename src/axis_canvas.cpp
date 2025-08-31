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

    auto bg_color    = app->get_color_bg();
    auto min_color   = app->get_color_min();
    auto max_color   = app->get_color_max();
    auto flat_color  = app->get_color_flat();
    auto value_color = app->get_color_value();
    auto fuzz_color  = app->get_color_fuzz();

    cr->set_source_rgb(bg_color.get_red(),
                       bg_color.get_green(),
                       bg_color.get_blue());
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

    // auto fg_color = style->get_color(get_state_flags());

    // cr->set_source_rgba(fg_color.get_red(),
    //                     fg_color.get_green(),
    //                     fg_color.get_blue(),
    //                     fg_color.get_alpha());

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
        cr->set_source_rgba(min_color.get_red(),
                            min_color.get_green(),
                            min_color.get_blue(),
                            min_color.get_alpha());
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);

        // max
        cr->set_source_rgba(max_color.get_red(),
                            max_color.get_green(),
                            max_color.get_blue(),
                            max_color.get_alpha());
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
        cr->set_source_rgba(min_color.get_red(),
                            min_color.get_green(),
                            min_color.get_blue(),
                            min_color.get_alpha());
        cr->arc(left + r, 0, r, M_PI/2, 3*M_PI/2);
        cr->stroke();

        // calc max
        cr->set_source_rgba(max_color.get_red(),
                            max_color.get_green(),
                            max_color.get_blue(),
                            max_color.get_alpha());
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
        cr->set_source_rgba(flat_color.get_red(),
                            flat_color.get_green(),
                            flat_color.get_blue(),
                            flat_color.get_alpha());
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
        cr->set_source_rgba(flat_color.get_red(),
                            flat_color.get_green(),
                            flat_color.get_blue(),
                            flat_color.get_alpha());
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

        cr->set_source_rgba(value_color.get_red(),
                            value_color.get_green(),
                            value_color.get_blue(),
                            value_color.get_alpha());
        cr->move_to(+r,  0);
        cr->line_to( 0, -r);
        cr->line_to(-r,  0);
        cr->line_to( 0, +r);
        cr->close_path();
        cr->fill();

        // draw fuzz
        cr->set_source_rgba(fuzz_color.get_red(),
                            fuzz_color.get_green(),
                            fuzz_color.get_blue(),
                            fuzz_color.get_alpha());
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
            cr->set_line_width(2.0);
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
