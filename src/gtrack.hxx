
#ifndef LUPPP_G_TRACK_H
#define LUPPP_G_TRACK_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "avtk/avtk_button.h"

class GTrack : public Fl_Group
{
  public:
    GTrack(int x, int y, int w, int h ) :
      Fl_Group(x, y, w, h),
      button(x + 5, y + 5, 100, 20,"Quit"),
      button2(x + 5, y + 25, 100, 20,"2"),
      button3(x + 5, y + 45, 100, 20,"3")
      //slider(x, y + 50, w, 20)
    {
      end(); // close the group
    }
    
  
  private:
    AvtkButton button;
    AvtkButton button2;
    AvtkButton button3;
};

#endif // LUPPP_G_TRACK_H

