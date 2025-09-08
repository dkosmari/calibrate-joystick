/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CONTROLLER_DB_HPP
#define CONTROLLER_DB_HPP

#include <compare>
#include <cstdint>
#include <map>
#include <string>
#include <utility>

#include <libevdevxx/AbsInfo.hpp>


namespace ControllerDB {

    struct Key {
        std::uint16_t vendor;
        std::uint16_t product;
        std::uint16_t version;
        std::string name;

        constexpr
        bool
        operator ==(const Key& other)
            const noexcept = default;

        constexpr
        std::strong_ordering
        operator <=>(const Key& other)
            const noexcept = default;
    };


    struct AxisData {
        evdev::AbsInfo info;
        bool flat_centered = false;
    };

    using DevConf = std::map<std::string, AxisData>;


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

    void
    remove(std::uint16_t vendor,
           std::uint16_t product,
           std::uint16_t version,
           const std::string& name);

    std::pair<const Key*, const DevConf*>
    find(std::uint16_t vendor,
         std::uint16_t product,
         std::uint16_t version,
         const std::string& name)
        noexcept;

} // namespace ControllerDB

#endif
