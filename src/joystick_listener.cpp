#include <iostream>

#include "joystick_listener.hpp"

#include "app.hpp"
#include "utils.hpp"


using std::string;
using std::cout;
using std::endl;

using gudev::Client;
using gudev::Device;


JoystickListener::JoystickListener(App& app) :
    Client{{"input"}},
    app(app)
{}


void
JoystickListener::on_uevent(const string& action,
                            const Device& device)
{
    if (auto subsystem = device.subsystem();
        !subsystem || *subsystem != "input")
        return;

    if (auto name = device.name();
        !name || !starts_with(*name, "event"))
        return;

    if (!device.property_as<bool>("ID_INPUT_JOYSTICK"))
        return;

    if (!device.has_tag("uaccess"))
        return;

    if (action == "add")
        app.add_device(device);
    else if (action == "remove")
        app.remove_device(device);
    else
        cout << "ignoring action: " << action << endl;
}
