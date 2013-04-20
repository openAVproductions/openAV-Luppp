
#ifndef AVTK_BUTTON_H
#define AVTK_BUTTON_H

#include <FL/Fl_Button.H>

class AvtkButton : public Fl_Button
{
  public:
    AvtkButton(int _x, int _y, int _w, int _h, const char *_label=0):
        Fl_Button(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      label = _label;
      
      highlight = false;
      mouseOver = false;
    }
    
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        cairo_save( cr );
        
        cairo_rectangle( cr, x+1, y+1, w-2, h-2 );
        cairo_set_source_rgb( cr,28 / 255.f,  28 / 255.f ,  28 / 255.f  );
        cairo_fill_preserve(cr);
        
        cairo_set_line_width(cr, 1.5);
        cairo_rectangle( cr, x+1, y+1, w-2, h-2 );
        
        if ( highlight )
        {
          cairo_set_source_rgba(cr, 1.0, 0.48,   0, 0.4);
          cairo_fill_preserve(cr);
        }
        
        float alpha = 0.7;
        if (mouseOver)
          alpha = 1;
        cairo_set_source_rgba(cr, 1.0, 0.48,   0, alpha);
        cairo_stroke(cr);
        
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
          if ( test_shortcut() )
          {
            do_callback();
            return 1;
          }
          return 0;
        default:
          return Fl_Widget::handle(event);
      }
    }
};

#endif // AVTK_BUTTON_H

