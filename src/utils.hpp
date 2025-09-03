/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <gtkmm.h>


extern const std::string ui_axis_info_path;
extern const std::string ui_device_page_path;


template<typename T>
std::unique_ptr<T>
get_widget(Glib::RefPtr<Gtk::Builder>& builder,
           const std::string& name)
{
    T* ptr = nullptr;
    builder->get_widget(name, ptr);
    if (!ptr)
        throw std::runtime_error{"widget \"" + name + "\" not found."};
    ptr->reference(); // keep the widget alive after the builder is destroyed
    return std::unique_ptr<T>{ptr};
}


template<typename T,
         typename... Args>
std::unique_ptr<T>
get_widget_derived(Glib::RefPtr<Gtk::Builder>& builder,
                   const std::string& name,
                   Args&&... args)
{
    T* ptr = nullptr;
    builder->get_widget_derived(name, ptr, std::forward<Args>(args)...);
    if (!ptr)
        throw std::runtime_error{"widget \"" + name + "\" not found."};
    ptr->reference(); // keep the widget alive after the builder is destroyed
    return std::unique_ptr<T>{ptr};
}



template<typename T>
T
variant_cast(const Glib::VariantBase& v)
{
    return Glib::VariantBase::cast_dynamic<Glib::Variant<T>>(v).get();
}


#endif
