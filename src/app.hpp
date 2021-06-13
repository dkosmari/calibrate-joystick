#ifndef APP_HPP
#define APP_HPP

// standard libraries
#include <memory>
#include <string>
#include <vector>

// system libraries
#include <gtkmm.h>
#include <gudevxx/client.hpp>

// local headers
#include "refresh_button.hpp"
#include "joystick_listener.hpp"


class DevicePage;


class App : public Gtk::Application {

    JoystickListener joystick_listener;

    std::unique_ptr<Gtk::ApplicationWindow> main_window;
    std::unique_ptr<Gtk::HeaderBar> header_bar;

    RefreshButton* refresh_button = nullptr;
    Gtk::Stack* device_stack = nullptr;

    std::vector<std::unique_ptr<DevicePage>> devices;


    void on_startup() override;

    void on_activate() override;

public:

    App();
    ~App();


    void create_main_window();


    void clear_devices();

    void add_device(const gudev::Device& d);

    void remove_device(const gudev::Device& d);


    void refresh_joysticks();

};


#endif
