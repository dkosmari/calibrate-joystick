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

#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <stdexcept>

#include "client.hpp"
#include "device.hpp"
#include "enumerator.hpp"

using std::cout;
using std::endl;
using std::string;
using std::cerr;
using std::runtime_error;
using std::pair;

using namespace std::literals;

using gudev::Client;
using gudev::Device;
using gudev::Enumerator;


void print(const Device& d)
{
    cout << "DEV:  "
         << d.name().value_or("")
         << "  :  "
         << d.device_file().value_or("").string()
         << "  :  "
         << d.sysfs().value_or("").string()
         << endl;
}


pair<string,string>
split_keyval(const string& text)
{
    auto p = text.find('=');
    if (p == string::npos)
        throw runtime_error{"Invalid key-value string: missing '='"};

    return { text.substr(0, p), text.substr(p+1) };
}


void enforce_plus(char type, char op)
{
    if (op != '+')
        throw runtime_error{"Invalid operation: only '+' is accepted for "s
                + type};
}


int   main(int argc, char* argv[])
try {
    Client cli;

    Enumerator etor{cli};

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.size() < 2)
            throw runtime_error{"\""s + arg + "\" is too short"};

        std::istringstream input{arg};
        char type = 0;
        char op = 0;
        string rest;

        if (!(input >> type >> op))
            throw runtime_error{"Failed to parse argument: "s + arg};

        getline(input, rest);

        switch (type) {

            case 's':
            case 'S':
                switch (op) {
                    case '+':
                        etor.match_subsystem(rest);
                        break;
                    case '-':
                        etor.nomatch_subsystem(rest);
                        break;
                    default:
                        throw runtime_error{"Invalid operation: "s + op};
                }
                break;

            case 'a':
            case 'A': {
                auto [key, val] = split_keyval(rest);
                switch (op) {
                    case '+':
                        etor.match_sysfs_attr(key, val);
                        break;
                    case '-':
                        etor.nomatch_sysfs_attr(key, val);
                        break;
                    default:
                        throw runtime_error{"Invalid operation: "s + op};
                        continue;
                }
                break;
            }

            case 'p':
            case 'P': {
                enforce_plus(type, op);
                auto [key, val] = split_keyval(rest);
                etor.match_property(key, val);
                break;
            }

            case 'n':
            case 'N':
                enforce_plus(type, op);
                etor.match_name(rest);
                break;

            case 't':
            case 'T':
                enforce_plus(type, op);
                etor.match_tag(rest);
                break;

            case 'i':
            case 'I':
                enforce_plus(type, op);
                etor.match_initialized();
                break;

            default:
                throw runtime_error{"Invalid argument type: "s + type};
        }

    }

    auto devs = etor.execute();
    for (auto& d : devs)
        print(d);
}
catch (std::exception& e) {
    cerr << "ERROR: " << e.what() << endl;
    return -1;
}
