/*
 * calibrate-joystick - a program to calibrate joysticks on Linux
 *
 * Copyright (C) 2025  Daniel K. O.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <iostream>

#include "settings.hpp"

#include "app.hpp"

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

    settings = Gio::Settings::create(APPLICATION_ID);

    settings->signal_changed().connect([this](const Glib::ustring& key)
    {
        if (key == "background-color")
            app->set_background_color(Gdk::RGBA{settings->get_string(key)});
        if (key == "value-color")
            app->set_value_color(Gdk::RGBA{settings->get_string(key)});
        if (key == "min-color")
            app->set_min_color(Gdk::RGBA{settings->get_string(key)});
        if (key == "max-color")
            app->set_max_color(Gdk::RGBA{settings->get_string(key)});
        if (key == "fuzz-color")
            app->set_fuzz_color(Gdk::RGBA{settings->get_string(key)});
        if (key == "flat-color")
            app->set_flat_color(Gdk::RGBA{settings->get_string(key)});
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

    // Set the initial values into the App class
    app->set_background_color(Gdk::RGBA{settings->get_string("background-color")});
    app->set_value_color(Gdk::RGBA{settings->get_string("value-color")});
    app->set_min_color(Gdk::RGBA{settings->get_string("min-color")});
    app->set_max_color(Gdk::RGBA{settings->get_string("max-color")});
    app->set_fuzz_color(Gdk::RGBA{settings->get_string("fuzz-color")});
    app->set_flat_color(Gdk::RGBA{settings->get_string("flat-color")});


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
