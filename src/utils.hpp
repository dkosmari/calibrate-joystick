#ifndef UTILS_HPP
#define UTILS_HPP

#include <memory>
#include <string>

#include <gtkmm.h>


extern const std::string ui_main_window_path;
extern const std::string ui_device_page_path;
extern const std::string ui_axis_info_path;


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
    ptr->reference();
    return std::unique_ptr<T>{ptr};
}


#endif
