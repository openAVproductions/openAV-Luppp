
#ifndef AVTK_WAVEFORM_H
#define AVTK_WAVEFORM_H

#include <FL/Fl_Widget.H>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace Cairo;

namespace Avtk
{

class Waveform : public Fl_Widget
{
  public:
    AvtkWaveform(int _x, int _y, int _w, int _h, const char *_label=0 ):
        Fl_Widget(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      label = _label;
      
      highlight = false;
      newWaveform = false;
      
      waveformCr = 0;
      waveformSurf = 0;
      
      
      data = new std::vector<float>();
      
      srand (time(NULL));
      
      for (int i = 0; i < 700; i++)
      {
        data->push_back( 0.f );
      }
      
      
      newWaveform = true;
      
      strokeRim = true;
    }
    
    bool strokeRim;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    cairo_t*          waveformCr;
    cairo_surface_t*  waveformSurf;
    
    bool newWaveform;
    
    std::vector<float>* data;
    
    void setDataPtr(std::vector<float>* d )
    {
      //cout << "AvtkWaveform: setDataPtr = " << data << endl;
      data = d;
      newWaveform = true;
      damage(FL_DAMAGE_ALL);
    }
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        //Cairo::Context cr = Cairo::Context( c_cr );
        //Cairo::Context cairoContext = Context( c_cr, false );
        //Cairo::Context* cr = &cairoContext;
        
        cairo_save(cr);
        
        // clear the surface
        cairo_rectangle(cr, x, y, w, h);
        cairo_set_source_rgb (cr, 0.1,0.1,0.1);
        cairo_fill( cr );
        
        
        //cout << "waveform have data" << endl;
        
        if ( newWaveform )
        {
          if ( !waveformCr )
          {
            // create the waveform surface and context
            //cout << "waveform draw() creating new objects" << endl;
            waveformSurf= cairo_image_surface_create ( CAIRO_FORMAT_ARGB32, w, h);
            waveformCr  = cairo_create ( waveformSurf );
            //cout << "waveform draw() creating objects done" << endl;
          }
          
          // clear the surface
          cairo_rectangle(waveformCr, 0, 0, w, h);
          cairo_set_source_rgb (waveformCr, 0.1,0.1,0.1);
          cairo_fill( waveformCr );
          
          // set up dashed lines, 1 px off, 1 px on
          double dashes[1];
          dashes[0] = 2.0;
          
          cairo_set_dash ( waveformCr, dashes, 1, 0.0);
          cairo_set_line_width( waveformCr, 1.0);
          
          // loop over each 2nd line, drawing dots
          cairo_set_line_width(waveformCr, 1.0);
          cairo_set_source_rgb(waveformCr, 0.4,0.4,0.4);
          for ( int i = 1; i < 4; i++ )
          {
            cairo_move_to( waveformCr, ((w / 4.f)*i), 0);
            cairo_line_to( waveformCr, ((w / 4.f)*i), h );
          }
          for ( int i = 1; i < 4; i++ )
          {
            cairo_move_to( waveformCr, 0, ((h / 4.f)*i) );
            cairo_line_to( waveformCr, w, ((h / 4.f)*i) );
          }
          
          cairo_set_source_rgba( waveformCr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
          cairo_stroke(waveformCr);
          cairo_set_dash ( waveformCr, dashes, 0, 0.0);
          
          
          if ( not data )
          {
            // draw X
            cairo_move_to( cr,  0    , 0     );
            cairo_line_to( cr,  0 + w, 0 + h );
            cairo_move_to( cr,  0    , 0 + h );
            cairo_line_to( cr,  0 + w, 0     );
            cairo_set_source_rgb ( cr, 0.2,0.2,0.2);
            cairo_stroke(cr);
            
            // draw text
            cairo_move_to( cr,  0 + (w/2.f) - 65, 0 + (h/2.f) + 10 );
            cairo_set_source_rgb ( cr, 0.6,0.6,0.6);
            cairo_set_font_size( cr, 20 );
            cairo_show_text( cr, "No data loaded" );
          }
          else
          {
            // don't draw every sample
            int sampleCountForDrawing = -1;
            
            float currentTop = 0.f;
            float previousTop = 0.f;
            float currentSample = 0.f;
            
            // find how many samples per pixel
            int samplesPerPix = data->size() / w;
            //cout << "width = " << w << "  sampsPerPx " << samplesPerPix << endl;
            
            // loop over each pixel value we need
            for( int p = 0; p < w; p++ )
            {
              float averageMax = 0.f;
              float averageMin = 0.f;
              
              // calc value for this pixel
              for( int i = 0; i < samplesPerPix; i++ )
              {
                float tmp = data->at(i + (p * samplesPerPix) );
                if ( tmp < 0 )
                {
                  averageMin += tmp;
                }
                else
                {
                  averageMax += tmp;
                }
              }
              
              // - 0.1 to keep in bounds of square
              averageMin = (averageMin / samplesPerPix);
              averageMax = (averageMax / samplesPerPix);
              
              float dH = h * 0.4;
              
              // draw lines
              cairo_move_to( waveformCr, p, h * 0.5 + (averageMin * dH ) );
              cairo_line_to( waveformCr, p, h * 0.5 + (averageMax * dH ) );
            }
            
            // stroke the waveform
            cairo_set_source_rgb( waveformCr, 1.0,1.0,1.0);
            cairo_stroke( waveformCr );
            
            newWaveform = false;
          }
        }
        
        cairo_set_source_surface(cr, waveformSurf, x, y);
        cairo_rectangle( cr, x, y, w, h);
        cairo_paint(cr);
        
        if ( strokeRim )
        {
          // stroke rim
          cairo_set_line_width(cr, 1);
          cairo_rectangle(cr, x, y, w, h);
          cairo_set_source_rgba( cr, 100 / 255.f, 100 / 255.f , 100 / 255.f , 1 );
          cairo_stroke( cr );
        }
        
        //cout << "waveform draw() done" << endl;
        
        cairo_restore(cr);
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
      return 0;
      
      switch(event)
      {
        case FL_PUSH:
          highlight = 0;
          redraw();
          return 1;
        case FL_DRAG: {
            int t = Fl::event_inside(this);
            if (t != highlight) {
              redraw();
            }
          }
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

} // Avtk

#endif

