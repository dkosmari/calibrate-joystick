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

#ifndef LIBGUDEVXX_GOBJECT_BASE_HPP_GUARD
#define LIBGUDEVXX_GOBJECT_BASE_HPP_GUARD

#include <memory>
#include <stdexcept>

#include <glib.h>


namespace gudev {

    
    template<typename T,
             typename Derived>
    class GObjectBase {

        struct Deleter {
            void operator()(T* obj) const
            {
                g_object_unref(obj);
            }
        };

        std::unique_ptr<T, Deleter> ptr;

        
    protected:
        
        GObjectBase(T* obj, bool steal) :
            ptr{obj}
        {
            if (!gobj())
                throw std::logic_error{"Constructing from a null pointer."};
            if (!steal)
                g_object_ref(gobj());
        }


    public:

        static
        Derived
        own(T* obj)
        {
            return Derived{obj, true};
        }


        static
        Derived
        view(T* obj)
        {
            return Derived{obj, false};
        }


        T* gobj() const
        {
            return ptr.get();
        }


        T* gobj()
        {
            return ptr.get();
        }

        
    };

}


#endif
