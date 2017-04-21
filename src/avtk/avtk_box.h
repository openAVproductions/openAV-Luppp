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


#ifndef AVTK_BOX_H
#define AVTK_BOX_H

#include <FL/Fl_Widget.H>

namespace Avtk
{

// FIXME: Refactor to .hxx .cxx
class Box : public Fl_Widget
{
public:
	Box(int _x, int _y, int _w, int _h, const char *_label = 0):
		Fl_Widget(_x, _y, _w, _h)
	{
		copy_label(_label);
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		r = g = b = 0;

		highlight = false;
	}
	bool highlight;
	int x, y, w, h;

	int r,g,b;

	void setColor( int red, int green, int blue )
	{
		r = red;
		g = green;
		b = blue;

		redraw();
	}

	void draw()
	{
		if (damage() & FL_DAMAGE_ALL) {
			cairo_t *cr = Fl::cairo_cc();

			cairo_save( cr );

			cairo_set_line_width(cr, 1.5);

			// fill background
			cairo_rectangle( cr, x, y, w, h);
			cairo_set_source_rgba( cr, r, g, b, 1 );
			cairo_fill( cr );

			/*
			// stroke rim
			cairo_rectangle(cr, x, y, w, h);
			cairo_set_source_rgba( cr, 66 / 255.f, 66 / 255.f , 66 / 255.f , 1 );
			cairo_stroke( cr );
			*/

			cairo_restore( cr );
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
};

} // Avtk

#endif // AVTK_BOX_H

