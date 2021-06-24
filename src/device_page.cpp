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
#include <utility>

#include "device_page.hpp"

#include "axis_info.hpp"
#include "utils.hpp"


using std::string;
using std::make_unique;
using std::filesystem::path;

using Glib::ustring;
using Glib::IOCondition;
using Gio::SimpleActionGroup;
using Glib::VariantBase;
using Glib::Variant;

using evdev::Type;
using evdev::Code;


DevicePage::DevicePage(const std::filesystem::path& dev_path) :
    dev_path{dev_path},
    device{dev_path}
{

    load_widgets();

    actions = SimpleActionGroup::create();
    root().insert_action_group("dev", actions);

    name_label->set_label(device.name());
    path_label->set_label(dev_path.string());

    auto abs_codes = device.codes(evdev::Type::abs);
    unsigned num_axes = abs_codes.size();

    axes_label->set_label(ustring::compose("%1", num_axes));

    for (auto code : abs_codes) {
        auto info = device.abs_info(code);
        auto [iter, inserted] = axes.emplace(code, make_unique<AxisInfo>(code, info));
        if (inserted)
            axes_box->pack_start(iter->second->root(),
                                 Gtk::PackOptions::PACK_SHRINK);
    }

    io_conn = Glib::signal_io().connect(sigc::mem_fun(this, &DevicePage::handle_io),
                                        device.fd(),
                                        IOCondition::IO_IN);

    actions->add_action("apply", sigc::mem_fun(this, &DevicePage::action_apply));
    actions->add_action("reset", sigc::mem_fun(this, &DevicePage::action_reset));


    auto arg_type = Glib::VariantType{G_VARIANT_TYPE_UINT16};
    actions->add_action_with_parameter("apply_axis",
                                       arg_type,
                                       sigc::mem_fun(this,
                                                     &DevicePage::action_apply_axis));

    actions->add_action_with_parameter("reset_axis",
                                       arg_type,
                                       sigc::mem_fun(this,
                                                     &DevicePage::action_reset_axis));
}


DevicePage::~DevicePage()
{
    io_conn.disconnect();
}


void
DevicePage::load_widgets()
{
    auto builder = Gtk::Builder::create_from_resource(ui_device_page_path);

    device_box = get_widget<Gtk::Box>(builder, "device_box");

    name_label = get_widget<Gtk::Label>(builder, "name_label");
    path_label = get_widget<Gtk::Label>(builder, "path_label");
    axes_label = get_widget<Gtk::Label>(builder, "axes_label");

    axes_box = get_widget<Gtk::Box>(builder, "axes_box");
}


Gtk::Widget&
DevicePage::root()
{
    if (!device_box)
        throw std::runtime_error{"failed to load root widget"};
    return *device_box;
}


string
DevicePage::name() const
{
    return device.name();
}


const path&
DevicePage::path() const
{
    return dev_path;
}


bool
DevicePage::handle_io(IOCondition cond)
{
    if (cond & IOCondition::IO_IN)
        handle_read();

    if (cond & (IOCondition::IO_HUP | IOCondition::IO_ERR))
        return false;

    return true;
}


void
DevicePage::handle_read()
{
    while (device.pending()) {
        auto event = device.read();
        if (event.type != Type::abs)
            continue;

        axes.at(event.code)->update_value(event.value);
    }
}


void
DevicePage::action_apply()
{
    for (auto& [code, _] : axes)
        apply_axis(code);
}


void
DevicePage::action_reset()
{
    for (auto& [code, _] : axes)
        reset_axis(code);
}


void
DevicePage::action_apply_axis(const Glib::VariantBase& arg)
{
    Code code = VariantBase::cast_dynamic<Variant<guint16>>(arg)
        .get();
    apply_axis(code);
}


void
DevicePage::action_reset_axis(const Glib::VariantBase& arg)
{
    Code code = VariantBase::cast_dynamic<Variant<guint16>>(arg)
        .get();
    reset_axis(code);
}


void
DevicePage::apply_axis(Code code)
{
    device.kernel_abs_info(code,
                           axes.at(code)->get_calc());
    reset_axis(code);
}


void
DevicePage::reset_axis(Code code)
{
    auto new_abs = device.abs_info(code);
    axes.at(code)->reset(new_abs);
}
