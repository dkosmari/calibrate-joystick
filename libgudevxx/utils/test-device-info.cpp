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
 * Test program to print device info.
 */

#include <chrono>
#include <filesystem>
#include <iostream>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

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
using std::istringstream;
using std::optional;
using std::ostringstream;
using std::string;

using Glib::OptionContext;
using Glib::OptionGroup;
using Glib::OptionEntry;
using gudev::Client;
using gudev::Device;


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
    const string indent(20, ' ');
    const string indent2(40, ' ');

    using std::to_string;
    using gudev::to_string;

    auto aprint = [&indent](const string& left, const string& right = "")
    {
        if (indent.size() > left.size()) {
            size_t pad_size = indent.size() - left.size();
            string padding = indent.substr(0, pad_size);
            cout << padding;
        }
        cout << left << right << "\n";
    };
    auto printif = [&aprint](const string& label, const optional<auto>& val)
    {
        if (val) {
            using T = std::remove_reference_t<
                std::remove_cv_t<decltype(val.value())>>;

            if constexpr(std::is_convertible_v<T, std::string>)
                aprint(label, *val);
            else
                aprint(label, to_string(*val));
        }
    };

    auto aprint2 = [&indent2](const string& left, const string& right = "")
    {
        if (indent2.size() > left.size()) {
            size_t pad_size = indent2.size() - left.size();
            string padding = indent2.substr(0, pad_size);
            cout << padding;
        }
        cout << left;
        if (!left.empty() && !right.empty())
            cout << " = ";
        else
            cout << "   ";
        cout << right << "\n";
    };

    cout << "Device:\n";
    printif("Subsystem: ", dev.subsystem());
    printif("Devtype: ", dev.devtype());
    printif("Name: ", dev.name());
    printif("Number: ", dev.number());
    printif("SysFS: ", dev.sysfs());
    printif("Driver: ", dev.driver());
    printif("Action: ", dev.action());
    printif("Seqnum: ", dev.seqnum());
    printif("Device type: ", optional{dev.type()});
    printif("Device number: ", dev.device_number());

    if (dev.initialized())
        aprint("Initialized: ",
               human_time(dev.since_initialized()) + " ago");

    printif("Device file: ", dev.device_file());

    if (auto symlinks = dev.device_symlinks(); !symlinks.empty()) {
        aprint("Symlinks: ", "");
        for (const auto& s : symlinks)
            aprint("", s.string());
    }

    if (auto tags = dev.tags(); !tags.empty()) {
        aprint("Tags: ");
        for (const auto& t : tags)
            aprint("", t);
    }

    // TODO: print with the same syntax as udev
    if (auto pkeys = dev.property_keys(); !pkeys.empty()) {
        aprint("Properties: ");
        for (const auto& k : pkeys)
            aprint2(k, dev.property_as<string>(k));
    }

    // TODO: print with the same syntax as udev
    if (auto akeys = dev.sysfs_attr_keys(); !akeys.empty()) {
        aprint("Sysfs attrs: ");
        for (const auto& k : akeys) {
            auto val = dev.sysfs_attr(k);
            if (val) {
                if (val->find('\n') == string::npos)
                    aprint2(k, *val);
                else {
                    aprint2(k, "{");
                    istringstream input{*val};
                    string line;
                    while (getline(input, line))
                        cout << indent2 << line << "\n";
                    aprint2("", "}");
                }
            } else {
                // only they key
                aprint2(k);
            }
        }
    }
}


int main(int argc, char* argv[])
try {
    std::locale::global(std::locale(""));

    Glib::init();

    Glib::ustring subsystem;
    Glib::ustring file;
    bool show_parents = false;

    OptionContext ctx;

    OptionGroup grp{"options", "Main options"};

    OptionEntry sub_opt;
    sub_opt.set_long_name("subsystem");
    sub_opt.set_short_name('s');
    sub_opt.set_arg_description("SUBSYSTEM");
    sub_opt.set_description("Only list from selected subsystem.");
    grp.add_entry(sub_opt, subsystem);

    OptionEntry dev_opt;
    dev_opt.set_long_name("device");
    dev_opt.set_short_name('d');
    dev_opt.set_arg_description("FILE");
    dev_opt.set_description("Print device info from specified FILE.");
    grp.add_entry(dev_opt, file);

    OptionEntry par_opt;
    par_opt.set_long_name("parents");
    par_opt.set_short_name('p');
    par_opt.set_description("Show parent devices.");
    grp.add_entry(par_opt, show_parents);

    ctx.set_main_group(grp);


    if (!ctx.parse(argc, argv)) {
        cerr << "Unable to parse options." << endl;
        return -1;
    }


    Client client;


    if (!file.empty()) {
        path dev_path = file.raw();
        auto dev_printer = [show_parents](const Device& d)
        {
            print_device(d);
            if (show_parents)
                for (auto p = d.parent(); p; p = p->parent()) {
                    cout << "\nParent:" << endl;
                    print_device(*p);
                }
        };

        if (auto d = client.get(dev_path))
            dev_printer(*d);
        else if (auto d = client.get_sysfs(dev_path))
            dev_printer(*d);
        else {
            cerr << "Could not access device " << dev_path << endl;
            return -1;
        }

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
