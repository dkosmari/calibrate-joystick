#include <iostream>

#include "app.hpp"


using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
    auto resources =
        Gio::Resource::create_from_file(RESOURCE_DIR
                                        "/resources.gresource");
    resources->register_global();

    App app;
    //cout << app.get_resource_base_path() << endl;

    return app.run(argc, argv);
}
