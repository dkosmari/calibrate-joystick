#include <algorithm>
#include <valarray>

#include "axis_canvas.hpp"


using std::valarray;

using evdev::AbsInfo;


AxisCanvas::AxisCanvas(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& /* builder */,
                       const AbsInfo& orig) :
    Gtk::DrawingArea{cobject},
    orig{orig},
    calc{orig}
{}


bool
AxisCanvas::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const double padding = 24;

    const double width = get_allocated_width();
    const double height = get_allocated_height();

    auto style = get_style_context();


    style->render_background(cr, 0, 0, width, height);


    double range = orig.max - orig.min;

    auto translate = [this, width, range, padding](double x) -> double
    {
        return padding + (x - orig.min) * double(width - 2 * padding) / range;
    };

    auto fg_color = style->get_color(get_state_flags());

    cr->set_source_rgba(fg_color.get_red(),
                        fg_color.get_green(),
                        fg_color.get_blue(),
                        fg_color.get_alpha());
    cr->set_line_width(1.5);


    {
        // draw orig min-max
        const double h = 10.5;
        const double w = 5.5;
        const double left = translate(orig.min);
        const double right = translate(orig.max);

        cr->save();
        cr->translate(0, height / 2.0);

        // min
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);

        // max
        cr->move_to(right - w, -h);
        cr->line_to(right, -h);
        cr->line_to(right, +h);
        cr->line_to(right - w, +h);

        cr->stroke();
        cr->restore();
    }


    {
        // draw calc min-max
        const double h = 6.5;
        const double w = 3.5;
        const double left = translate(calc.min);
        const double right = translate(calc.max);

        cr->save();
        cr->translate(0, height / 2.0);

        // calc min
        cr->move_to(left + w, -h);
        cr->line_to(left, -h);
        cr->line_to(left, +h);
        cr->line_to(left + w, +h);

        // calc max
        cr->move_to(right - w, -h);
        cr->line_to(right, -h);
        cr->line_to(right, +h);
        cr->line_to(right - w, +h);

        cr->stroke();
        cr->restore();
    }


    {
        // draw orig flat
        const double h = 9.5;
        const double left = translate(-orig.flat);
        const double w = translate(orig.flat) - left;
        cr->save();
        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->set_dash(valarray{2.0, 2.0}, 0);
        cr->rectangle(left, -h, w, 2 * h);
        cr->stroke();
        cr->restore();
    }


    {
        // draw calc flat
        const double h = 6.5;
        const double left = translate(-calc.flat);
        const double w = translate(calc.flat) - left;
        cr->save();
        cr->translate(0, height / 2.0);
        cr->set_line_width(1.0);
        cr->rectangle(left, -h, w, 2 * h);
        cr->stroke();
        cr->restore();
    }


    {
        // draw value marker
        const double r = 2.5;
        cr->save();
        cr->translate(translate(calc.val), height / 2.0);

        cr->move_to(+r,  0);
        cr->line_to( 0, -r);
        cr->line_to(-r,  0);
        cr->line_to( 0, +r);
        cr->close_path();
        cr->fill();

        cr->set_line_width(1.0);

        const double fs = 3.5;
        {
            // orig fuzz
            const double ow = translate(orig.fuzz) - translate(0);
            const double oh = std::min<double>(fs, ow);
            // left
            cr->move_to(-ow + oh, -oh);
            cr->line_to(-ow, 0);
            cr->line_to(-ow + oh, +oh);
            // right
            cr->move_to(ow - oh, -oh);
            cr->line_to(ow, 0);
            cr->line_to(ow - oh, +oh);
        }


        {
            // calc fuzz
            const double cw = translate(calc.fuzz) - translate(0);
            const double ch = std::min<double>(fs, cw);
            // left
            cr->move_to(-cw + ch, -ch);
            cr->line_to(-cw, 0);
            cr->line_to(-cw + ch, +ch);
            // right
            cr->move_to(cw - ch, -ch);
            cr->line_to(cw, 0);
            cr->line_to(cw - ch, +ch);
        }


        cr->stroke();
        cr->restore();
    }

    return true;
}


void
AxisCanvas::update(const AbsInfo& new_calc)
{
    calc = new_calc;
    queue_draw();
}
