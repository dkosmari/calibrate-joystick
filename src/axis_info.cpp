/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cstdint>
#include <stdexcept>
#include <iostream>

#include "axis_info.hpp"

#include "app.hpp"
#include "axis_canvas.hpp"
#include "utils.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glibmm/i18n.h>


using std::string;
using std::cout;
using std::endl;

using Glib::ustring;
using Gio::SimpleActionGroup;
using Glib::Variant;

using evdev::AbsInfo;


AxisInfo::AxisInfo(evdev::Code axis_code,
                   const AbsInfo& info) :
    code{axis_code}
{
    load_widgets();
    name_label->set_label(evdev::code_to_string(evdev::Type::abs, axis_code));

    actions = SimpleActionGroup::create();
    root().insert_action_group("axis", actions);

    // NOTE: since "this" is captured, this object is not movable
#define CONN_SYNC_SPIN(x)                                               \
    x ## _changed_conn = calc_ ## x ## _spin->signal_value_changed()    \
        .connect([this] {                                               \
            calc.x = calc_ ## x ## _spin->get_value_as_int();           \
            update_ ## x (calc.x);                                      \
        })

    CONN_SYNC_SPIN(min);
    CONN_SYNC_SPIN(max);
    CONN_SYNC_SPIN(fuzz);
    CONN_SYNC_SPIN(flat);
    CONN_SYNC_SPIN(res);

#undef CONN_SYNC_SPIN

    action_apply = actions->add_action("apply",
                                       sigc::mem_fun(this, &AxisInfo::on_action_apply));

    action_revert = actions->add_action("revert",
                                        sigc::mem_fun(this, &AxisInfo::on_action_revert));

    reset(info);
}


void
AxisInfo::load_widgets()
{
    auto builder = Gtk::Builder::create_from_resource(ui_axis_info_path);

    info_frame = get_widget<Gtk::Frame>(builder, "info_frame");

    builder->get_widget("name_label", name_label);

    builder->get_widget("value_label", value_label);

    builder->get_widget("orig_min_label",  orig_min_label);
    builder->get_widget("orig_max_label",  orig_max_label);
    builder->get_widget("orig_fuzz_label", orig_fuzz_label);
    builder->get_widget("orig_flat_label", orig_flat_label);
    builder->get_widget("orig_res_label",  orig_res_label);

    builder->get_widget("calc_min_spin",  calc_min_spin);
    builder->get_widget("calc_max_spin",  calc_max_spin);
    builder->get_widget("calc_fuzz_spin", calc_fuzz_spin);
    builder->get_widget("calc_flat_spin", calc_flat_spin);
    builder->get_widget("calc_res_spin",  calc_res_spin);

    builder->get_widget_derived("axis_canvas", axis_canvas, orig);
    update_canvas();

    builder->get_widget("flat_item_zero", flat_item_zero);
    builder->get_widget("flat_item_centered", flat_item_centered);
    // Note: GtkRadioMenuItem does not support actions, so we use signals.
    flat_item_zero    ->signal_toggled().connect([this]{ on_changed_flat_to_zero(); });
    flat_item_centered->signal_toggled().connect([this]{ on_changed_flat_to_centered(); });
}


Gtk::Widget&
AxisInfo::root()
{
    if (!info_frame)
        throw std::runtime_error{"Failed to load root widget."};
    return *info_frame;
}


void
AxisInfo::update_canvas()
{
    if (axis_canvas)
        axis_canvas->update(calc);
}


void
AxisInfo::update_value(int value)
{
    value_label->set_label(ustring::format(value));

    if (value < calc.min)
        calc.min = value;
    if (value > calc.max)
        calc.max = value;

    calc_min_spin->set_value(calc.min);
    calc_max_spin->set_value(calc.max);

    calc.val = value;

    update_canvas();
}


void
AxisInfo::update_min(int min)
{
    calc.min = min;
    update_canvas();
}


void
AxisInfo::update_max(int max)
{
    calc.max = max;
    update_canvas();
}


void
AxisInfo::update_fuzz(int fuzz)
{
    calc.fuzz = fuzz;
    update_canvas();
}


void
AxisInfo::update_flat(int flat)
{
    calc.flat = flat;
    update_canvas();
}


void
AxisInfo::update_res(int res)
{
    calc.res = res;
    update_canvas();
}


void
AxisInfo::on_action_apply()
{
    root().get_action_group("dev")
        ->activate_action("apply_axis", Variant<guint16>::create(code));
}


void
AxisInfo::on_action_revert()
{
    root().get_action_group("dev")
        ->activate_action("revert_axis", Variant<guint16>::create(code));
}


void
AxisInfo::on_changed_flat_to_zero()
{
    if (!flat_item_zero->get_active())
        return;
    if (axis_canvas)
        axis_canvas->set_flat_centered(false);
}


void
AxisInfo::on_changed_flat_to_centered()
{
    if (!flat_item_centered->get_active())
        return;
    if (axis_canvas)
        axis_canvas->set_flat_centered(true);
}


const AbsInfo&
AxisInfo::get_calc() const noexcept
{
    return calc;
}


void
AxisInfo::reset(const AbsInfo& new_orig)
{
    calc = orig = new_orig;
    calc.min = calc.max = orig.val;

    orig_min_label ->set_label(ustring::format(orig.min));
    orig_max_label ->set_label(ustring::format(orig.max));
    orig_fuzz_label->set_label(ustring::format(orig.fuzz));
    orig_flat_label->set_label(ustring::format(orig.flat));
    orig_res_label ->set_label(ustring::format(orig.res));

    calc_fuzz_spin->set_value(calc.fuzz);
    calc_flat_spin->set_value(calc.flat);
    calc_res_spin ->set_value(calc.res);

    update_value(calc.val);

    if (axis_canvas)
        axis_canvas->reset(orig, calc);
}


void
AxisInfo::disable()
{
    action_apply->set_enabled(false);
    action_revert->set_enabled(false);

    calc_min_spin->set_sensitive(false);
    calc_max_spin->set_sensitive(false);
    calc_fuzz_spin->set_sensitive(false);
    calc_flat_spin->set_sensitive(false);
    calc_res_spin->set_sensitive(false);
}


void
AxisInfo::update_colors(const App* app)
{
    axis_canvas->set_background_color(app->get_background_color());
    axis_canvas->set_value_color(app->get_value_color());
    axis_canvas->set_min_color(app->get_min_color());
    axis_canvas->set_max_color(app->get_max_color());
    axis_canvas->set_fuzz_color(app->get_fuzz_color());
    axis_canvas->set_flat_color(app->get_flat_color());
    update_canvas();
}
