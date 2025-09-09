/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cmath>
#include <iostream>

#include "settings.hpp"

#include "app.hpp"
#include "axis_canvas.hpp"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


using std::cout;
using std::endl;


namespace {

    // Code to convert between GdkRGBA and strings for GSettings

    GVariant*
    rgba_to_string(const GValue* value,
                   const GVariantType* /*expected_type*/,
                   gpointer /*user_data*/)
    {
        if (!G_VALUE_HOLDS(value, GDK_TYPE_RGBA))
            return g_variant_new_string("");
        auto rgba = reinterpret_cast<const GdkRGBA*>(g_value_get_boxed(value));
        auto str = gdk_rgba_to_string(rgba);
        return g_variant_new_take_string(str);
    }


    gboolean
    string_to_rgba(GValue* value,
                   GVariant* variant,
                   gpointer /*user_data*/)
    {
        auto str = g_variant_get_string(variant, nullptr);
        if (!str)
            return false;
        auto rgba = g_new(GdkRGBA, 1);
        g_value_take_boxed(value, rgba);
        return gdk_rgba_parse(rgba, str);
    }

} // namespace


void
Settings::on_show()
{
    Gtk::ApplicationWindow::on_show();

    sample_time = 0;
    sample_info_orig.val = 0;
    sample_info_orig.min = -1024;
    sample_info_orig.max = 1024;
    sample_info_orig.fuzz = 128;
    sample_info_orig.flat = 128;
    sample_info_calc = sample_info_orig;
    sample_info_calc.min = -800;
    sample_info_calc.max = 800;
    sample_info_calc.fuzz = 200;
    sample_info_calc.flat = 250;

    sample_axis_canvas->reset(sample_info_orig, sample_info_calc);

    sample_timeout_handle = Glib::signal_timeout()
        .connect(sigc::mem_fun(this, &Settings::animate_axis_sample), 33);
}


void
Settings::on_hide()
{
    sample_timeout_handle.disconnect();
    Gtk::ApplicationWindow::on_hide();
}


namespace {

    template<typename T,
             typename U>
    T
    lerp(T low,
         T high,
         U ratio)
    {
        return static_cast<T>((1 - ratio) * low + ratio * high);
    }

    template<typename T>
    T
    square(T x)
    {
        return x*x;
    }

} // namespace


bool
Settings::animate_axis_sample()
{
    double x = std::cos(10 * sample_time) * square(std::sin(sample_time));
    sample_info_calc.val = lerp(sample_info_calc.min,
                                sample_info_calc.max,
                                0.5 + x / 2.0);
    sample_time += 0.002 * M_PI;
    if (sample_time >= M_PI)
        sample_time = -M_PI;
    sample_axis_canvas->update(sample_info_calc);
    return true;
}


Settings::Settings(BaseObjectType* cobject,
                   const Glib::RefPtr<Gtk::Builder>& builder,
                   App* app_) :
    Gtk::ApplicationWindow{cobject},
    app{app_}
{
    builder->get_widget("background_color_button", background_color_button);
    builder->get_widget("value_color_button", value_color_button);
    builder->get_widget("min_color_button", min_color_button);
    builder->get_widget("max_color_button", max_color_button);
    builder->get_widget("fuzz_color_button", fuzz_color_button);
    builder->get_widget("flat_color_button", flat_color_button);

    builder->get_widget_derived("sample_axis_canvas",
                                sample_axis_canvas,
                                sample_info_orig);
    g_assert(sample_axis_canvas);

    settings = Gio::Settings::create(APPLICATION_ID);

    settings->signal_changed().connect(
        [this](const Glib::ustring& key)
        {
            if (key == "background-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_background_color(val);
                sample_axis_canvas->set_background_color(val);
            }
            if (key == "value-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_value_color(val);
                sample_axis_canvas->set_value_color(val);
            }
            if (key == "min-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_min_color(val);
                sample_axis_canvas->set_min_color(val);
            }
            if (key == "max-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_max_color(val);
                sample_axis_canvas->set_max_color(val);
            }
            if (key == "fuzz-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_fuzz_color(val);
                sample_axis_canvas->set_fuzz_color(val);
            }
            if (key == "flat-color") {
                auto val = Gdk::RGBA{settings->get_string(key)};
                app->set_flat_color(val);
                sample_axis_canvas->set_flat_color(val);
            }
        });

    g_settings_bind_with_mapping(settings->gobj(), "background-color",
                                 background_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);
    g_settings_bind_with_mapping(settings->gobj(), "value-color",
                                 value_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);
    g_settings_bind_with_mapping(settings->gobj(), "min-color",
                                 min_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);
    g_settings_bind_with_mapping(settings->gobj(), "max-color",
                                 max_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);
    g_settings_bind_with_mapping(settings->gobj(), "fuzz-color",
                                 fuzz_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);
    g_settings_bind_with_mapping(settings->gobj(), "flat-color",
                                 flat_color_button->gobj(), "rgba",
                                 G_SETTINGS_BIND_DEFAULT,
                                 string_to_rgba, rgba_to_string,
                                 nullptr, nullptr);

    auto initialize_colors = [](auto target,
                                const Glib::RefPtr<Gio::Settings>& settings)
    {
        target->set_background_color(Gdk::RGBA{settings->get_string("background-color")});
        target->set_value_color(Gdk::RGBA{settings->get_string("value-color")});
        target->set_min_color(Gdk::RGBA{settings->get_string("min-color")});
        target->set_max_color(Gdk::RGBA{settings->get_string("max-color")});
        target->set_fuzz_color(Gdk::RGBA{settings->get_string("fuzz-color")});
        target->set_flat_color(Gdk::RGBA{settings->get_string("flat-color")});
    };
    // Set the initial values into the App class
    initialize_colors(app, settings);
    // Do the same with sample_axis_canvas
    initialize_colors(sample_axis_canvas, settings);


    add_action("close", sigc::mem_fun(this, &Settings::on_action_close));
    add_action("reset", sigc::mem_fun(this, &Settings::on_action_reset));
}


void
Settings::on_action_close()
{
    hide();
}


void
Settings::on_action_reset()
{
    settings->reset("background-color");
    settings->reset("value-color");
    settings->reset("min-color");
    settings->reset("max-color");
    settings->reset("fuzz-color");
    settings->reset("flat-color");
}
