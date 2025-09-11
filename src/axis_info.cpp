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

#include "axis_canvas.hpp"
#include "utils.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glibmm/i18n.h>


using std::cout;
using std::endl;
using std::string;

using Glib::ustring;
using Glib::Variant;

using evdev::AbsInfo;


namespace {

    const string axis_info_glade = RESOURCE_PREFIX "/ui/axis-info.glade";

} // namespace


AxisInfo::AxisInfo(evdev::Code axis_code,
                   const AbsInfo& info) :
    code{axis_code}
{
    load_widgets();
    create_actions();

    name_label->set_label(evdev::code_to_string(evdev::Type::abs, axis_code));
    reset(info);
}


void
AxisInfo::create_actions()
{
    actions = Gio::SimpleActionGroup::create();
    root().insert_action_group("axis", actions);

    action_apply = actions->add_action("apply",
                                       sigc::mem_fun(this, &AxisInfo::on_action_apply));

    action_revert = actions->add_action("revert",
                                        sigc::mem_fun(this, &AxisInfo::on_action_revert));
}


void
AxisInfo::load_widgets()
{
    auto builder = Gtk::Builder::create_from_resource(axis_info_glade);

    utils::get_widget(builder, "info_frame", info_frame);

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

    calc_min_spin->signal_value_changed()
        .connect([this] { set_calc_min(calc_min_spin->get_value_as_int()); });
    calc_max_spin->signal_value_changed()
        .connect([this] { set_calc_max(calc_max_spin->get_value_as_int()); });
    calc_fuzz_spin->signal_value_changed()
        .connect([this] { set_calc_fuzz(calc_fuzz_spin->get_value_as_int()); });
    calc_flat_spin->signal_value_changed()
        .connect([this] { set_calc_flat(calc_flat_spin->get_value_as_int()); });
    calc_res_spin->signal_value_changed()
        .connect([this] { set_calc_res(calc_res_spin->get_value_as_int()); });

    builder->get_widget_derived("axis_canvas", axis_canvas, orig);
    update_canvas();

    builder->get_widget("flat_item_zero", flat_item_zero);
    builder->get_widget("flat_item_centered", flat_item_centered);
    // Note: GtkRadioMenuItem does not support actions, so we use signals.
    flat_item_zero    ->signal_toggled().connect([this] { on_changed_flat_to_zero(); });
    flat_item_centered->signal_toggled().connect([this] { on_changed_flat_to_centered(); });
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
AxisInfo::set_calc_value(int value)
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
AxisInfo::set_calc_min(int min)
{
    calc.min = min;
    update_canvas();
}


void
AxisInfo::set_calc_max(int max)
{
    calc.max = max;
    update_canvas();
}


void
AxisInfo::set_calc_fuzz(int fuzz)
{
    calc.fuzz = fuzz;
    update_canvas();
}


void
AxisInfo::set_calc_flat(int flat)
{
    calc.flat = flat;
    update_canvas();
}


void
AxisInfo::set_calc_res(int res)
{
    calc.res = res;
    update_canvas();
}


void
AxisInfo::on_action_apply()
{
    root().get_action_group("dev")->activate_action("apply_axis",
                                                    Variant<guint16>::create(code));
}


void
AxisInfo::on_action_revert()
{
    root().get_action_group("dev")->activate_action("revert_axis",
                                                    Variant<guint16>::create(code));
}


void
AxisInfo::on_changed_flat_to_zero()
{
    if (!flat_item_zero->get_active())
        return;
    flat_centered = false;
    if (axis_canvas)
        axis_canvas->set_flat_centered(false);
}


void
AxisInfo::on_changed_flat_to_centered()
{
    if (!flat_item_centered->get_active())
        return;
    flat_centered = true;
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

    set_calc_value(calc.val);

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
AxisInfo::set_colors(const Colors& c)
{
    axis_canvas->set_colors(c);
    update_canvas();
}


void
AxisInfo::set_flat_centered(bool centered)
{
    // dispatch this to the action handlers
    if (centered)
        flat_item_centered->set_active(true);
    else
        flat_item_zero->set_active(true);
}


bool
AxisInfo::is_flat_centered()
    const
{
    return flat_centered;
}
