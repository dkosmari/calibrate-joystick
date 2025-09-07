/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONTROLLER_DB_HPP
#define CONTROLLER_DB_HPP

#include <cstdint>
#include <map>
#include <string>

#include <libevdevxx/AbsInfo.hpp>


namespace ControllerDB {

    using DevConf = std::map<std::string, evdev::AbsInfo>;


    void
    initialize();

    void
    finalize()
        noexcept;


    void
    save(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name,
         const DevConf& configs);


    const DevConf*
    find(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name)
        noexcept;

} // namespace ControllerDB

#endif
