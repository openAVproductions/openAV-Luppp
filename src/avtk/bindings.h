
#ifndef AVTK_BINDINGS_H
#define AVTK_BINDINGS_H

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

namespace Avtk
{

class Bindings : public Fl_Button
{
  public:
    Bindings( int _x, int _y, int _w, int _h, const char *_label = 0 );
    
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void draw();
    //int handle(int event);
    void resize(int X, int Y, int W, int H);
};

}

#endif // AVTK_BINDINGS_H
