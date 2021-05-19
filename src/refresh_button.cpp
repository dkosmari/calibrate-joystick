#include <iostream>

#include "refresh_button.hpp"

#include "app.hpp"


RefreshButton::RefreshButton(BaseObjectType* cobject,
                             const Glib::RefPtr<Gtk::Builder>& /* builder */,
                             App& app) :
    Gtk::Button{cobject},
    app(app)
{}


void
RefreshButton::on_clicked()
{
    app.refresh_joysticks();
}
