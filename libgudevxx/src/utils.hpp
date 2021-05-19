/*
 *  libgudevxx - a C++ wrapper for libgudev
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

#ifndef LIBGUDEVXX_UTILS_HPP_GUARD
#define LIBGUDEVXX_UTILS_HPP_GUARD


#include <cstddef>
#include <vector>

#include <glib.h>


namespace gudev::utils {


    template<typename T>
    std::vector<T>
    gobj_list_to_vector(GList* list)
    {
        try  {
            std::vector<T> result;
            for (GList* i = list; i; i = i->next)
                result.emplace_back(reinterpret_cast<T>(i->data));
            g_list_free(list);
            return result;
        }
        catch (...) {
            for (GList* i = list; i; i = i->next)
                g_object_unref(i->data);
            g_list_free(list);
            throw;
        }
    }


    template<typename T = std::string>
    inline
    std::vector<T>
    strv_to_vector(const char* const strv[])
    {
        std::vector<T> result;
        for (std::size_t i = 0; strv[i]; ++i)
            result.push_back(strv[i]);
        return result;
    }


} // namespace gudev::utils


#endif
