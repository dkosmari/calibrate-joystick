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
