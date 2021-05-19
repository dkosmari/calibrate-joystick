#ifndef APP_HPP
#define APP_HPP

// standard libraries
#include <memory>
#include <string>
#include <vector>

// system libraries
#include <gtkmm.h>

// local libraries
#include "client.hpp"

// local headers
#include "device_page.hpp"
#include "refresh_button.hpp"
#include "joystick_listener.hpp"


class App : public Gtk::Application {

    JoystickListener joystick_listener;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;

    RefreshButton* refresh_button = nullptr;
    Gtk::Stack* device_stack = nullptr;


    std::vector<DevicePage> devices;

public:

    App();


    void create_main_window();

    void on_activate() override;


    void clear_devices();

    void add_device(const gudev::Device& d);

    void remove_device(const gudev::Device& d);


    void refresh_joysticks();

};


#endif
