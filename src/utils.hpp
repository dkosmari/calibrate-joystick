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


#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include <gtkmm.h>


extern const std::string ui_about_dialog_path;
extern const std::string ui_axis_info_path;
extern const std::string ui_device_page_path;
extern const std::string ui_main_window_path;


bool
starts_with(const std::string& haystack,
            const std::string& prefix);


template<typename T>
std::unique_ptr<T>
get_widget(Glib::RefPtr<Gtk::Builder>& builder,
           const std::string& name)
{
    T* ptr = nullptr;
    builder->get_widget(name, ptr);
    if (!ptr)
        throw std::runtime_error{"widget \"" + name + "\" not found."};
    ptr->reference();
    return std::unique_ptr<T>{ptr};
}


template<typename T>
T
variant_cast(const Glib::VariantBase& v)
{
    return Glib::VariantBase::cast_dynamic<Glib::Variant<T>>(v).get();
}


#endif
