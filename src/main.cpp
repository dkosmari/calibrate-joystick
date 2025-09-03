/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iostream>
#include <exception>
#include <clocale>


#include "app.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <glibmm/i18n.h>


int main(int argc, char* argv[])
{
    std::setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);

    try {
        App app;
        return app.run(argc, argv);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}
