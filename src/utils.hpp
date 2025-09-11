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

namespace utils {

    template<typename T>
    void
    get_widget(Glib::RefPtr<Gtk::Builder>& builder,
               const std::string& name,
               std::unique_ptr<T>& result)
    {
        T* ptr = nullptr;
        builder->get_widget(name, ptr);
        if (!ptr)
            throw std::runtime_error{"widget \"" + name + "\" not found."};
        ptr->reference(); // keep the widget alive after the builder is destroyed
        result.reset(ptr);
    }


    template<typename T,
             typename... Args>
    void
    get_widget_derived(Glib::RefPtr<Gtk::Builder>& builder,
                       const std::string& name,
                       std::unique_ptr<T>& result,
                       Args&&... args)
    {
        T* ptr = nullptr;
        builder->get_widget_derived(name, ptr, std::forward<Args>(args)...);
        if (!ptr)
            throw std::runtime_error{"widget \"" + name + "\" not found."};
        ptr->reference(); // keep the widget alive after the builder is destroyed
        result.reset(ptr);
    }


    template<typename T>
    T
    variant_cast(const Glib::VariantBase& v)
    {
        return Glib::VariantBase::cast_dynamic<Glib::Variant<T>>(v).get();
    }

} // namespace utils

#endif
