#ifndef AXIS_CANVAS_HPP
#define AXIS_CANVAS_HPP

#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include <libevdevxx/abs_info.hpp>


class AxisCanvas : public Gtk::DrawingArea {

    evdev::AbsInfo orig;
    evdev::AbsInfo calc;


    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

public:

    AxisCanvas(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& /* builder */,
               const evdev::AbsInfo& orig);


    void update(const evdev::AbsInfo& new_calc);
};


#endif
