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

/*
 *
 */

#include <chrono>
#include <filesystem>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include <glibmm.h>

#include "client.hpp"
#include "device.hpp"


using std::cerr;
using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::minutes;
using std::chrono::seconds;
using std::cout;
using std::endl;
using std::filesystem::path;
using std::ostringstream;
using std::string;

using Glib::OptionContext;
using Glib::OptionGroup;
using Glib::OptionEntry;
using gudevxx::Client;
using gudevxx::Device;


template<typename Rep, typename Ratio>
string human_time(std::chrono::duration<Rep, Ratio> t)
{
    ostringstream out;

    if (auto u = duration_cast<microseconds>(t); u.count() <= 5000)
        out << u.count() << " µs";
    else if (auto m = duration_cast<milliseconds>(t); m.count() < 5000)
        out << m.count() << " ms";
    else if (auto s = duration_cast<seconds>(t); s.count() < 120)
        out << s.count() << " s";
    else if (auto m = duration_cast<minutes>(t); m.count() < 120)
        out << m.count() << " m";
    else
        out << duration_cast<hours>(t).count() << " h";
    return out.str();
}


void print_device(const Device& dev)
{
    cout << "Device:\n";
    if (auto subsystem = dev.subsystem())
        cout << "      Subsystem: " << *subsystem << "\n";
    if (auto devtype = dev.devtype())
        cout << "        Devtype: " << *devtype << "\n";
    if (auto name = dev.name())
        cout << "           Name: " << *name << "\n";
    if (auto number = dev.number())
        cout << "         Number: " << *number << "\n";
    if (auto sysfs = dev.sysfs())
        cout << "          SysFS: " << sysfs->string() << "\n";
    if (auto driver = dev.driver())
        cout << "         Driver: " << *driver << "\n";
    if (auto action = dev.action())
        cout << "         Action: " << *action << "\n";

    if (auto seqnum = dev.seqnum())
        cout << "         Seqnum: " << *seqnum << "\n";
    cout << "    Device type: " << dev.type() << "\n";
    if (auto devnum = dev.device_number())
        cout << "  Device number: " << *devnum << "\n";
    if (dev.initialized())
        cout << "    Initialized: "
             << human_time(dev.since_initialized())
             << " ago\n";

    if (auto device_file = dev.device_file())
        cout << "    Device File: " << device_file->string() << "\n";

    if (auto symlinks = dev.device_symlinks(); !symlinks.empty()) {
        cout << "       Symlinks: ";
        const char* sep = "";
        for (const auto& s : symlinks) {
            cout << sep << s.string() << "\n";
            sep = "                 ";
        }
    }

    if (auto tags = dev.tags(); !tags.empty()) {
        cout << "           Tags: ";
        const char* sep = "";
        for (const auto& t : tags) {
            cout << sep << t;
            sep = ", ";
        }
        cout << "\n";
    }

    if (auto pkeys = dev.property_keys(); !pkeys.empty()) {
        cout << "     Properties: ";
        const char* sep = "";
        for (const auto& k : pkeys) {
            cout << sep << k << " = " << dev.property_as<string>(k) << "\n";
            sep = "                 ";
        }
    }

    if (auto akeys = dev.sysfs_attr_keys(); !akeys.empty()) {
        cout << "    Sysfs attrs: ";
        const char* sep = "";
        for (const auto& k : akeys) {
            if (k == "uevent")
                continue;
            cout << sep << k;
            auto val = dev.sysfs_attr(k);
            if (val)
                cout << " = " << val.value();
            cout << "\n";
            sep = "                 ";
        }
    }
}




int main(int argc, char* argv[])
try {
    std::locale::global(std::locale(""));

    Glib::init();

    bool show_help = false;
    Glib::ustring subsystem;
    Glib::ustring file;
    bool show_parents = false;

    OptionContext ctx;
    ctx.set_help_enabled(false);

    OptionGroup grp{"options", "Main options"};

    OptionEntry help_opt;
    help_opt.set_long_name("help");
    help_opt.set_short_name('h');
    help_opt.set_description("Show help.");
    grp.add_entry(help_opt, show_help);

    OptionEntry sub_opt;
    sub_opt.set_long_name("subsystem");
    sub_opt.set_short_name('s');
    sub_opt.set_arg_description("SUBSYSTEM");
    sub_opt.set_description("Only list from selected subsystem.");
    grp.add_entry(sub_opt, subsystem);

    OptionEntry file_opt;
    file_opt.set_long_name("file");
    file_opt.set_short_name('f');
    file_opt.set_arg_description("FILE");
    file_opt.set_description("Print device info from specified FILE.");
    grp.add_entry(file_opt, file);


    ctx.set_main_group(grp);


    if (!ctx.parse(argc, argv)) {
        cerr << "Unable to parse options." << endl;
        return -1;
    }

    if (show_help) {
        cout << ctx.get_help();
        return 0;
    }


    Client client;


    if (!file.empty()) {
        Device d = client.get(path{file.raw()}).value();
        print_device(d);
        return 0;
    }


    // no remaining arguments, so just list all devices
    if (argc == 1) {
        auto devices = client.query(subsystem);
        for (auto& d : devices)
            cout << d.subsystem().value_or("")
                 << " : "
                 << d.device_number().value_or(0)
                 << " : "
                 << d.device_file().value_or("").string()
                 << " : "
                 << d.sysfs().value_or("").string()
                 << endl;
        return 0;
    }


}
catch (Glib::Exception& e) {
    cerr << e.what() << endl;
    return -2;
}
catch (std::exception& e) {
    cerr << e.what() << endl;
    return -1;
}
