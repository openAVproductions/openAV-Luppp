/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef AVTK_RADIAL_STATUS_H
#define AVTK_RADIAL_STATUS_H

#include <FL/Fl_Slider.H>

namespace Avtk
{

class RadialStatus : public Fl_Slider
{
public:
	RadialStatus(int _x, int _y, int _w, int _h, const char *_label = 0):
		Fl_Slider(_x, _y, _w, _h, _label)
	{
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		label = _label;

		previousAngle = -1;

		_r = 1.0;
		_g = 0.48;
		_b = 0.0;

		_bgr = 0.11;
		_bgg = 0.11;
		_bgb = 0.11;

		highlight = false;
		mouseOver = false;

		_recording = false;
	}

	bool _recording;
	void recording(bool r)
	{
		_recording = r;
		redraw();
	}

	bool mouseOver;
	bool highlight;
	int x, y, w, h;
	const char* label;

	int previousAngle;

	float _r, _g, _b;       // foreground colour
	float _bgr, _bgg, _bgb; // background colour
	float _outr, _outg, _outb; // outline colour

	void setColor(float r, float g, float b)
	{
		_r = r;
		_g = g;
		_b = b;
	}

	void setBgColor(float r, float g, float b)
	{
		_bgr = r;
		_bgg = g;
		_bgb = b;
	}

	void draw()
	{
		// check that its a new "segment" to redraw
		int newAngle = (value() * 16);

		if (damage() & FL_DAMAGE_ALL &&
		    previousAngle != newAngle ) {
			cairo_t *cr = Fl::cairo_cc();

			cairo_save( cr );

			int xc = (w) / 2.f;
			int yc = (h) / 2.f;

			int radius = xc > yc ? yc : xc;
			radius -= 10;

			float angle = newAngle / 16.f;

			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND );
			//cairo_move_to( cr, x + xc, y + yc );
			cairo_arc( cr, x + xc, y + yc, radius, -(3.1415/2), angle * 6.28 - (3.1415/2) );
			cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.5 );
			cairo_set_line_width(cr, 12);
			cairo_stroke_preserve(cr);

			cairo_set_source_rgba (cr, 1.0, 0.48, 0.0, 1 );
			cairo_set_line_width(cr, 8.2);
			cairo_stroke(cr);


			// recording?
			if ( _recording ) {
				cairo_set_source_rgba(cr,1.0,0.0,0.0, 0.8);
				cairo_arc(cr, x + xc, y + yc, radius-4, 0, 2 * 3.1415);
				cairo_fill_preserve(cr);
				cairo_set_source_rgba(cr,0.0,0.0,0.0, 0.8);
				cairo_set_line_width(cr, 2.2);
				cairo_stroke(cr);

				// color of internal dot depending on rec state
				cairo_set_source_rgba(cr,0.0,0.0,0.0, 1);
			} else {
				cairo_set_source_rgba(cr,0.3,0.3,0.3, 1);
			}

			// inside circle
			cairo_arc(cr, x + xc, y + yc, 10, 0, 2 * 3.1415);
			cairo_set_line_width(cr, 4.2);
			cairo_fill(cr);

			cairo_restore( cr );

			draw_label();
		}
	}

	void resize(int X, int Y, int W, int H)
	{
		Fl_Widget::resize(X,Y,W,H);
		x = X;
		y = Y;
		w = W;
		h = H;
		redraw();
	}

	int handle(int event)
	{
		switch(event) {
		case FL_PUSH:
			highlight = 1;
			redraw();
			return 1;
		case FL_DRAG: {
			int t = Fl::event_inside(this);
			if (t != highlight) {
				highlight = t;
				redraw();
			}
		}
		return 1;
		case FL_ENTER:
			mouseOver = true;
			redraw();
			return 1;
		case FL_LEAVE:
			mouseOver = false;
			redraw();
			return 1;
		case FL_RELEASE:
			if (highlight) {
				highlight = 0;
				redraw();
				do_callback();
			}
			return 1;
		case FL_SHORTCUT:
			if ( test_shortcut() ) {
				do_callback();
				return 1;
			}
			return 0;
		default:
			return Fl_Widget::handle(event);
		}
	}
};

} // Avtk

#endif // AVTK_RADIAL_STATUS_H

