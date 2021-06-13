#include <algorithm>
#include <iostream>
#include <utility>

#include "device_page.hpp"

#include "axis_info.hpp"
#include "utils.hpp"


using std::cout;
using std::endl;
using std::string;
using std::make_unique;
using std::filesystem::path;

using Glib::ustring;
using Glib::IOCondition;

using evdev::Type;


DevicePage::DevicePage(const std::filesystem::path& dev_path) :
    dev_path{dev_path},
    device{dev_path}
{
    load_widgets();

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
