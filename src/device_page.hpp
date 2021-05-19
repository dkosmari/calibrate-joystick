#ifndef DEVICE_PAGE_HPP
#define DEVICE_PAGE_HPP

#include <memory>
#include <string>
#include <filesystem>

#include <gtkmm.h>

#include "device.hpp"


struct DevicePage {
    std::filesystem::path dev_file;
    std::unique_ptr<Gtk::Box> box;
    std::unique_ptr<Gtk::Label> name_label;
    std::unique_ptr<Gtk::Label> path_label;

    DevicePage(const gudev::Device& device);

};


#endif
