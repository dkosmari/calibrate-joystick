#include "device_page.hpp"

#include "utils.hpp"

using std::string;

using gudev::Device;


DevicePage::DevicePage(const Device& device) :
    dev_file{device.device_file().value()}
{
    auto builder = Gtk::Builder::create_from_file(ui_device_page_path);
    box = get_widget<Gtk::Box>(builder, "device_page");
    name_label = get_widget<Gtk::Label>(builder, "name_label");
    path_label = get_widget<Gtk::Label>(builder, "path_label");
}
