#ifndef AXIS_INFO_HPP
#define AXIS_INFO_HPP


#include <memory>
#include <string>

#include <gtkmm.h>
#include <libevdevxx/abs_info.hpp>
#include <libevdevxx/event.hpp>


class AxisCanvas;


class AxisInfo {

    template<typename T>
    using ptr = std::unique_ptr<T>;

    using LabelPtr = ptr<Gtk::Label>;
    using SpinButtonPtr = ptr<Gtk::SpinButton>;


    ptr<Gtk::Frame> info_frame;
    LabelPtr name_label;

    LabelPtr value_label;
    LabelPtr orig_min_label;
    LabelPtr orig_max_label;
    LabelPtr orig_fuzz_label;
    LabelPtr orig_flat_label;
    LabelPtr orig_res_label;

    SpinButtonPtr calc_min_spin;
    SpinButtonPtr calc_max_spin;
    SpinButtonPtr calc_fuzz_spin;
    SpinButtonPtr calc_flat_spin;
    SpinButtonPtr calc_res_spin;

    AxisCanvas* axis_canvas;

    evdev::AbsInfo orig;
    evdev::AbsInfo calc;

    sigc::connection min_changed_conn;
    sigc::connection max_changed_conn;
    sigc::connection fuzz_changed_conn;
    sigc::connection flat_changed_conn;
    sigc::connection res_changed_conn;


    void load_widgets();

    void update_canvas();

    void update_min(int min);
    void update_max(int max);
    void update_fuzz(int fuzz);
    void update_flat(int flat);
    void update_res(int res);

public:

    AxisInfo(evdev::Code axis_code,
             const evdev::AbsInfo& info);

    // disable moving
    AxisInfo(AxisInfo&& other) = delete;

    void update_value(int value);

    Gtk::Widget& root();

};


#endif
