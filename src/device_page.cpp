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
#include <iostream>
#include <utility>

#include "device_page.hpp"

#include "axis_info.hpp"
#include "utils.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glibmm/i18n.h>


using std::cout;
using std::endl;
using std::string;
using std::make_unique;
using std::filesystem::path;

using Glib::ustring;
using Glib::IOCondition;
using Gio::SimpleActionGroup;
using Glib::VariantBase;
using Glib::Variant;

using Type = evdev::Type;
using Code = evdev::Code;


DevicePage::DevicePage(const std::filesystem::path& dev_path) :
    dev_path{dev_path},
    device{dev_path}
{

    load_widgets();

    actions = SimpleActionGroup::create();
    root().insert_action_group("dev", actions);

    name_label->set_label(device.name());
    path_label->set_label(dev_path.string());

    auto abs_codes = device.codes(Type::abs);
    unsigned num_axes = abs_codes.size();

    axes_label->set_label(ustring::compose("%1", num_axes));

    for (auto code : abs_codes) {
        auto info = device.abs_info(code);
        auto [iter, inserted] = axes.emplace(code, make_unique<AxisInfo>(code, info));
        if (inserted)
            axes_box->pack_start(iter->second->root(),
                                 Gtk::PackOptions::PACK_SHRINK);
    }

    io_conn = Glib::signal_io().connect(sigc::mem_fun(this, &DevicePage::on_io),
                                        device.fd(),
                                        IOCondition::IO_IN |
                                        IOCondition::IO_ERR |
                                        IOCondition::IO_HUP);

    apply_action =
        actions->add_action("apply",
                            sigc::mem_fun(this, &DevicePage::on_action_apply));

    reset_action =
        actions->add_action("reset",
                            sigc::mem_fun(this, &DevicePage::on_action_reset));


    auto arg_type = Glib::VariantType{G_VARIANT_TYPE_UINT16};

    apply_axis_action =
        actions->add_action_with_parameter("apply_axis",
                                           arg_type,
                                           sigc::mem_fun(this,
                                                         &DevicePage::on_action_apply_axis));

    reset_axis_action =
        actions->add_action_with_parameter("reset_axis",
                                           arg_type,
                                           sigc::mem_fun(this,
                                                         &DevicePage::on_action_reset_axis));
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

    info_bar = get_widget<Gtk::InfoBar>(builder, "info_bar");
    error_label = get_widget<Gtk::Label>(builder, "error_label");
}


Gtk::Widget&
DevicePage::root()
{
    if (!device_box)
        throw std::runtime_error{"Failed to load root widget."};
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
DevicePage::on_io(IOCondition cond)
{
    if (cond & (IOCondition::IO_HUP | IOCondition::IO_ERR)) {
        device.close();
        disable();

        if (cond & IOCondition::IO_HUP)
            error_label->set_text(_("Device disconnected."));
        else
            error_label->set_text(_("Input/output error."));
        info_bar->show();
        info_bar->set_property("revealed", true);

        return false;
    }

    if (cond & IOCondition::IO_IN)
        handle_read();


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
DevicePage::on_action_apply()
{
    for (auto& [code, _] : axes)
        apply_axis(code);
}


void
DevicePage::on_action_reset()
{
    for (auto& [code, _] : axes)
        reset_axis(code);
}


void
DevicePage::on_action_apply_axis(const Glib::VariantBase& arg)
{
    Code code{variant_cast<guint16>(arg)};
    apply_axis(code);
}


void
DevicePage::on_action_reset_axis(const Glib::VariantBase& arg)
{
    Code code{variant_cast<guint16>(arg)};
    reset_axis(code);
}


void
DevicePage::apply_axis(Code code)
{
    if (!device.is_open())
        return;

    device.kernel_abs_info(code,
                           axes.at(code)->get_calc());
    reset_axis(code);
}


void
DevicePage::reset_axis(Code code)
{
    if (!device.is_open())
        return;

    auto new_abs = device.abs_info(code);
    axes.at(code)->reset(new_abs);
}


void
DevicePage::disable()
{
    apply_action->set_enabled(false);
    reset_action->set_enabled(false);
    apply_axis_action->set_enabled(false);
    reset_axis_action->set_enabled(false);

    for (auto& [_, axis] : axes)
        axis->disable();
}
