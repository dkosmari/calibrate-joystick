//#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstdint>

#include "axis_info.hpp"

#include "axis_canvas.hpp"
#include "utils.hpp"


using std::cout;
using std::endl;
using std::string;

using Glib::ustring;
using Gio::SimpleActionGroup;
using Glib::Variant;

using evdev::AbsInfo;


AxisInfo::AxisInfo(evdev::Code axis_code,
                   const AbsInfo& info) :
    code{axis_code}
{
    load_widgets();
    name_label->set_label(evdev::to_string(evdev::Type::abs, axis_code));

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

    actions->add_action("apply", sigc::mem_fun(this, &AxisInfo::action_apply));
    actions->add_action("reset", sigc::mem_fun(this, &AxisInfo::action_reset));

#undef CONN_SYNC_SPIN

    reset(info);
}


#define LOAD(t, n) \
    n = get_widget<Gtk:: t>(builder, #n)

void
AxisInfo::load_widgets()
{
    auto builder = Gtk::Builder::create_from_resource(ui_axis_info_path);

    LOAD(Frame, info_frame);

    LOAD(Label, name_label);

    LOAD(Label, value_label);

    LOAD(Label, orig_min_label);
    LOAD(Label, orig_max_label);
    LOAD(Label, orig_fuzz_label);
    LOAD(Label, orig_flat_label);
    LOAD(Label, orig_res_label);

    LOAD(SpinButton, calc_min_spin);
    LOAD(SpinButton, calc_max_spin);
    LOAD(SpinButton, calc_fuzz_spin);
    LOAD(SpinButton, calc_flat_spin);
    LOAD(SpinButton, calc_res_spin);

    builder->get_widget_derived("axis_canvas",
                                axis_canvas,
                                orig);
}

#undef LOAD


Gtk::Widget&
AxisInfo::root()
{
    if (!info_frame)
        throw std::runtime_error{"failed to load root widget"};
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
AxisInfo::action_apply()
{
    cout << "AxisInfo::action_apply(): " << code << endl;
    root().get_action_group("dev")
        ->activate_action("apply_axis", Variant<guint16>::create(code));
}


void
AxisInfo::action_reset()
{
    cout << "AxisInfo::action_reset(): " << code << endl;
    root().get_action_group("dev")
        ->activate_action("reset_axis", Variant<guint16>::create(code));
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

    orig_min_label->set_label(ustring::format(orig.min));
    orig_max_label->set_label(ustring::format(orig.max));
    orig_fuzz_label->set_label(ustring::format(orig.fuzz));
    orig_flat_label->set_label(ustring::format(orig.flat));
    orig_res_label->set_label(ustring::format(orig.res));

    calc_fuzz_spin->set_value(calc.fuzz);
    calc_flat_spin->set_value(calc.flat);
    calc_res_spin->set_value(calc.res);

    update_value(calc.val);

    if (axis_canvas)
        axis_canvas->reset(orig, calc);
}
