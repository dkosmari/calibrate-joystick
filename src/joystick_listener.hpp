#ifndef JOYSTICK_LISTENER_HPP
#define JOYSTICK_LISTENER_HPP

#include <gudevxx/client.hpp>


class App;


class JoystickListener : public gudev::Client {

    App& app;

public:

    JoystickListener(App& app);

    void on_uevent(const std::string& action,
                   const gudev::Device& device) override;

};

#endif
