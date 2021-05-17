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

/*------------------------------------------------------.
| This is a test program for the gudevxx::Client class. |
|                                                       |
| It acts like 'udevadm monitor'.                       |
|                                                       |
| Usage:                                                |
|                                                       |
|     ./test-listener [subsystem ...]                   |
|                                                       |
| Examples:                                             |
|                                                       |
|     ./test-listener                                   |
|                                                       |
|     ./test-listener input                             |
|                                                       |
|     ./test-listener usb bluetooth                     |
`------------------------------------------------------*/


#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <signal.h>

#include <glib.h>
#include <glib-unix.h>
#include <glibmm.h>

#include "client.hpp"


using std::cout;
using std::endl;
using std::flush;
using std::string;
using std::vector;
using std::ostringstream;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::minutes;
using std::chrono::hours;
using std::chrono::duration_cast;


using gudevxx::Client;
using gudevxx::Device;


gboolean handle_sigint(Glib::MainLoop* loop)
{
    cout << "\nCaught Ctrl-C." << endl;
    loop->quit();
    return false;
}


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


void print_event(GUdevClient* client_,
                 gchar* action_,
                 GUdevDevice* /*device_*/,
                 gpointer /*user_data*/)
{
    Client client = Client::view(client_);
    string action = action_;

    cout << "Client: " << client.gobj() << " : "
         << action << endl;

    cout << endl;
}


int main(int argc, char* argv[])
{
    Glib::init();

    vector<string> filters;
    for (int i=1; i<argc; ++i)
        filters.push_back(argv[i]);

    auto main_loop = Glib::MainLoop::create();

    g_unix_signal_add(SIGINT,
                      G_SOURCE_FUNC(handle_sigint),
                      main_loop.get());


    Client client = Client::listener(filters);

    g_signal_connect(client.gobj(),
                     "uevent",
                     G_CALLBACK(print_event),
                     nullptr);

    main_loop->run();

    cout << "exiting" << endl;
}
