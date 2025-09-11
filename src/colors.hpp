/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef COLORS_HPP
#define COLORS_HPP

#include <gdkmm/rgba.h>

struct Colors {

    Gdk::RGBA background;
    Gdk::RGBA value;
    Gdk::RGBA min;
    Gdk::RGBA max;
    Gdk::RGBA fuzz;
    Gdk::RGBA flat;

}; // struct Colors

#endif
