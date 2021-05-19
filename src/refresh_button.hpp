#ifndef REFRESH_BUTTON_HPP
#define REFRESH_BUTTON_HPP

#include <gtkmm/builder.h>
#include <gtkmm/button.h>


class App;


class RefreshButton : public Gtk::Button {

    App& app;

public:

    RefreshButton(BaseObjectType* cobject,
                  const Glib::RefPtr<Gtk::Builder>& /* builder */,
                  App& app);


    void on_clicked() override;
};


#endif
