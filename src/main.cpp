#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gtkmm.h>
#include <gudev/gudev.h>


#include "utils.hpp"


using Glib::RefPtr;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;
using std::vector;
using std::make_unique;
using std::size_t;
using std::pair;


//using Glib::ustring;

Gtk::Stack* device_stack;

const char* subsystems[] = { "input", nullptr };


const char* ui_main_window_path = "ui/main-window.glade";
const char* ui_device_page_path = "ui/device-page.glade";




template<typename T>
unique_ptr<T>
get_widget(RefPtr<Gtk::Builder>& builder,
           const string& name)
{
    T* ptr = nullptr;
    builder->get_widget(name, ptr);
    ptr->reference();
    return unique_ptr<T>(ptr);
}



// Current task: load each page from a builder

struct DevicePage {
    string path;
    unique_ptr<Gtk::Box> box;
    unique_ptr<Gtk::Label> name_label;
    unique_ptr<Gtk::Label> path_label;

    DevicePage(const string& path) :
        path{path}
    {
        auto builder = Gtk::Builder::create_from_file(ui_device_page_path);
        box = get_widget<Gtk::Box>(builder, "device_page");
        name_label = get_widget<Gtk::Label>(builder, "name_label");
        path_label = get_widget<Gtk::Label>(builder, "path_label");
    }

};

vector<DevicePage> devices;



namespace std {

    template<>
    struct default_delete<GUdevClient> {
        void operator()(GUdevClient* cli) const
        {
            g_object_unref(cli);
        }
    };

    template<>
    struct default_delete<GUdevDevice> {
        void operator()(GUdevDevice* dev) const
        {
            g_object_unref(dev);
        }
    };

}


using g_udev_client_t = unique_ptr<GUdevClient>;
using g_udev_device_t = unique_ptr<GUdevDevice>;


g_udev_client_t gudev_client;



vector<g_udev_device_t>
get_joysticks(g_udev_client_t& client)
{
    vector<g_udev_device_t> result;

    GList* devlist = g_udev_client_query_by_subsystem(client.get(), "input");

    for (auto item = devlist; item; item = item->next) {
        g_udev_device_t dev{G_UDEV_DEVICE(item->data)};
        item->data = nullptr;

        if (!g_udev_device_get_property_as_int(dev.get(),
                                               "ID_INPUT_JOYSTICK"))
            continue;

        const char* dev_path = g_udev_device_get_device_file(dev.get());
        if (!dev_path)
            continue;

        const char* dev_name = g_udev_device_get_name(dev.get());
        if (!dev_name || !starts_with(dev_name, "event"))
            continue;

        result.push_back(std::move(dev));
    }
    g_list_free(devlist);

    return result;
}



void clear_devices()
{
    /*
    for (auto& d: devices)
        device_stack->remove(*d.frame);
    */
    devices.clear();
}


void add_device(const string& path, const string& name)
{
    cout << "adding " << name << endl;
    devices.emplace_back(path);
    auto& dev = devices.back();
    dev.box->show_all();
    device_stack->add(*dev.box, name, name);
}


void remove_device(const string& path, const string& name)
{
    cout << "removing " << name << endl;
    for (size_t idx = 0; idx < devices.size(); ++idx)
        if (devices[idx].path == path) {
            devices.erase(devices.begin()+idx);
            return;
        }
}



/* -------------- */
/* event handlers */
/* -------------- */


extern "C"
G_MODULE_EXPORT
void refresh_button_clicked_cb(GtkToolButton*)
{
    cout << "refresh" << endl;
    clear_devices();
    auto joys = get_joysticks(gudev_client);
    for (const auto& [path, name] : joys)
        add_device(path, name);
}


void
handle_client_uevent(GUdevClient* /*client*/,
                     const gchar* action_,
                     GUdevDevice* device,
                     gpointer)
{
    const char* name = g_udev_device_get_name(device);
    const char* path = g_udev_device_get_device_file(device);
    if (!name)
        return;
    if (!path)
        return;

    string action = action_;

    if (action == "add")
        add_device(path, name);
    else if (action == "remove")
        remove_device(path, name);

}






int main(int argc, char* argv[])
{
    auto app =
        Gtk::Application::create(argc, argv,
                                 "org.calibrate-joystick");


    auto builder = Gtk::Builder::create_from_file(ui_main_window_path);
    gtk_builder_connect_signals(builder->gobj(), nullptr);

    auto window = get_widget<Gtk::ApplicationWindow>(builder,
                                                     "main_window");

    auto header_bar = get_widget<Gtk::HeaderBar>(builder,
                                                         "header_bar");
    window->set_titlebar(*header_bar);


    builder->get_widget("device_stack", device_stack);


    gudev_client.reset(g_udev_client_new(subsystems));
    g_signal_connect(gudev_client.get(),
                     "uevent",
                     G_CALLBACK(handle_client_uevent),
                     nullptr);


    builder.reset();


    int e = app->run(*window);

    return e;
}
