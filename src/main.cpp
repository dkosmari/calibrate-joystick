#include <iostream>

#include "app.hpp"


using std::cerr;
using std::cout;
using std::endl;

using Gio::Resource;


Glib::RefPtr<Resource>
load_resources()
{
    if (auto r = Resource::create_from_file("resources.gresource"))
        return r;

    if (auto r = Resource::create_from_file(RESOURCE_DIR "/resources.gresource"))
        return r;

    return {};
}


int main(int argc, char* argv[])
{
    auto resources = load_resources();
    if (!resources) {
        cerr << "could not load resources.gresource" << endl;
        return -1;
    }
    resources->register_global();

    App app;

    return app.run(argc, argv);
}
