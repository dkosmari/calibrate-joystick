/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <algorithm>
#include <exception>
#include <iostream>
#include <utility>

#include "device_page.hpp"

#include "app.hpp"
#include "axis_info.hpp"
#include "controller_db.hpp"
#include "utils.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glibmm/i18n.h>


using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::make_unique;
using std::filesystem::path;

using namespace std::literals;

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

    setup_actions();

    name_label->set_label(device.get_name());
    path_label->set_label(dev_path.string());

    vendor_label->set_label(ustring::sprintf("%04x", device.get_vendor()));
    product_label->set_label(ustring::sprintf("%04x", device.get_product()));
    version_label->set_label(ustring::sprintf("%04x", device.get_version()));

    auto abs_codes = device.get_codes(Type::abs);
    for (auto code : abs_codes) {
        auto info = device.get_abs_info(code);
        auto [iter, inserted] = axes.emplace(code, make_unique<AxisInfo>(code, info));
        if (inserted)
            axes_box->pack_start(iter->second->root(),
                                 Gtk::PackOptions::PACK_SHRINK);
    }

    try_load_config();

    io_conn = Glib::signal_io().connect(sigc::mem_fun(this, &DevicePage::on_io),
                                        device.get_fd(),
                                        IOCondition::IO_IN |
                                        IOCondition::IO_ERR |
                                        IOCondition::IO_HUP);
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

    builder->get_widget("path_label", path_label);
    builder->get_widget("name_label", name_label);
    builder->get_widget("vendor_label", vendor_label);
    builder->get_widget("product_label", product_label);
    builder->get_widget("version_label", version_label);

    builder->get_widget("name_check", name_check);
    builder->get_widget("vendor_check", vendor_check);
    builder->get_widget("product_check", product_check);
    builder->get_widget("version_check", version_check);

    builder->get_widget("axes_box", axes_box);
    builder->get_widget("info_bar", info_bar);
    builder->get_widget("error_label", error_label);
}


void
DevicePage::setup_actions()
{
    actions = SimpleActionGroup::create();
    root().insert_action_group("dev", actions);

    save_action =
        actions->add_action("save",
                            sigc::mem_fun(this, &DevicePage::on_action_save));

    delete_action =
        actions->add_action("delete",
                            sigc::mem_fun(this, &DevicePage::on_action_delete));

    apply_all_action =
        actions->add_action("apply_all",
                            sigc::mem_fun(this, &DevicePage::on_action_apply_all));

    revert_all_action =
        actions->add_action("revert_all",
                            sigc::mem_fun(this, &DevicePage::on_action_revert_all));


    apply_axis_action =
        actions->add_action_with_parameter("apply_axis",
                                           Glib::VariantType{G_VARIANT_TYPE_UINT16},
                                           sigc::mem_fun(this,
                                                         &DevicePage::on_action_apply_axis));

    revert_axis_action =
        actions->add_action_with_parameter("revert_axis",
                                           Glib::VariantType{G_VARIANT_TYPE_UINT16},
                                           sigc::mem_fun(this,
                                                         &DevicePage::on_action_revert_axis));
}


Gtk::Widget&
DevicePage::root()
{
    if (!device_box)
        throw std::runtime_error{"Failed to load root widget."};
    return *device_box;
}


string
DevicePage::name()
    const
{
    return device.get_name();
}


const path&
DevicePage::path()
    const
{
    return dev_path;
}


bool
DevicePage::on_io(IOCondition cond)
{
    if (cond & (IOCondition::IO_HUP | IOCondition::IO_ERR)) {
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
    while (device.has_pending()) {
        auto event = device.read();
        if (event.type != Type::abs)
            continue;

        axes.at(event.code)->update_value(event.value);
    }
}


void
DevicePage::on_action_save()
{
    try {
        auto vendor = vendor_check->get_active() ? device.get_vendor() : 0;
        auto product = product_check->get_active() ? device.get_product() : 0;
        auto version = version_check->get_active() ? device.get_version() : 0;
        auto name = name_check->get_active() ? device.get_name() : ""s;
        ControllerDB::DevConf conf;
        for  (const auto& [axis, ainfo] : axes) {
            auto& data = conf[code_to_string(evdev::Type::abs, axis)];
            data.info = device.get_abs_info(axis);
            data.flat_centered = ainfo->is_flat_centered();
        }
        ControllerDB::save(vendor, product, version, name, std::move(conf));
    }
    catch (std::exception& e) {
        cerr << "Failed to save: " << e.what() << endl;
    }
}


void
DevicePage::on_action_delete()
{
    try {
        auto vendor = vendor_check->get_active() ? device.get_vendor() : 0;
        auto product = product_check->get_active() ? device.get_product() : 0;
        auto version = version_check->get_active() ? device.get_version() : 0;
        auto name = name_check->get_active() ? device.get_name() : ""s;
        ControllerDB::remove(vendor, product, version, name);
    }
    catch (std::exception& e) {
        cerr << "Failed to remove: " << e.what() << endl;
    }
}


void
DevicePage::on_action_apply_all()
{
    for (auto& [code, _] : axes)
        apply_axis(code);
}


void
DevicePage::on_action_revert_all()
{
    for (auto& [code, _] : axes)
        revert_axis(code);
}


void
DevicePage::on_action_apply_axis(const Glib::VariantBase& arg)
{
    Code code{variant_cast<guint16>(arg)};
    apply_axis(code);
}


void
DevicePage::on_action_revert_axis(const Glib::VariantBase& arg)
{
    Code code{variant_cast<guint16>(arg)};
    revert_axis(code);
}


void
DevicePage::apply_axis(Code code)
{
    if (!device.is_open())
        return;

    device.set_kernel_abs_info(code,
                               axes.at(code)->get_calc());
    revert_axis(code);
}


void
DevicePage::revert_axis(Code code)
{
    if (!device.is_open())
        return;

    auto new_abs = device.get_abs_info(code);
    axes.at(code)->reset(new_abs);
}


void
DevicePage::disable()
{
    save_action->set_enabled(false);
    delete_action->set_enabled(false);
    apply_all_action->set_enabled(false);
    revert_all_action->set_enabled(false);
    apply_axis_action->set_enabled(false);
    revert_axis_action->set_enabled(false);

    for (auto& [_, axis] : axes)
        axis->disable();
}


void
DevicePage::try_load_config()
{
    auto [key, conf] = ControllerDB::find(device.get_vendor(),
                                          device.get_product(),
                                          device.get_version(),
                                          device.get_name());
    if (!key || !conf)
        return;

    for (const auto& [code_name, data] : *conf) {
        auto [type, code] = evdev::Code::parse(code_name);
        axes.at(code)->set_flat_centered(data.flat_centered);
        // Note: don't feed a fake zero .val to the kernel nor to the axis_info children.
        evdev::AbsInfo new_info = data.info;
        new_info.val = device.get_abs_info(code).val;
        device.set_kernel_abs_info(code, new_info);
        // cout << "Resetting axis " << code_to_string(type, code) << " to " << new_info << endl;
        axes.at(code)->reset(new_info);
    }

    // Activate checkbuttons based on what the matching key has.
    vendor_check->set_active(!!key->vendor);
    product_check->set_active(!!key->product);
    version_check->set_active(!!key->version);
    name_check->set_active(!key->name.empty());

    cout << "Applied config file for " << device.get_name() << endl;
}


void
DevicePage::update_colors(const App* app)
{
    for (auto& [key, val] : axes)
        val->update_colors(app);
}
