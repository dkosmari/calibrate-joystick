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


using gudevmm::Client;
using gudevmm::Device;


gboolean handle_sigint(Glib::MainLoop* loop)
{
    cout << "Caught Ctrl+C." << endl;
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

    cout << "         Seqnum: " << dev.seqnum() << "\n";
    cout << "    Device type: " << dev.device_type() << "\n";
    cout << "  Device number: " << dev.device_number() << "\n";
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

    // TODO: print parents if a command line option was given

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

#if 1
    if (auto akeys = dev.sysfs_attr_keys(); !akeys.empty()) {
        cout << "    Sysfs attrs: ";
        const char* sep = "";
        for (const auto& k : akeys) {
            cout << sep << k;
            auto val = dev.sysfs_attr(k);
            if (val)
                cout << " = " << val.value();
            cout << "\n";
            sep = "                 ";
        }
    }
#endif
}


void print_event(GUdevClient* client_,
                 gchar* action_,
                 GUdevDevice* device_,
                 gpointer /*user_data*/)
{
    Client client = Client::view(client_);
    string action = action_;

    cout << "Client: " << client.gobj() << " : "
         << action << endl;
    print_device(Device::view(device_));

    cout << endl;
}


int main(int argc, char* argv[])
{
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
