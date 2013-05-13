
#ifndef LUPPP_G_TRACK_H
#define LUPPP_G_TRACK_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "avtk/avtk_dial.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_background.h"

class GTrack : public Fl_Group
{
  public:
    GTrack(int x, int y, int w, int h, const char* l = 0 ) :
      Fl_Group(x, y, w, h),
      title( strdup(l) ),
      bg( x, y , w, h, title ),
      
      button1(x + 5, y + 24, 18, 18,"1"),
      button2(x + 5, y + 44, 18, 18,"2"),
      button3(x + 5, y + 64, 18, 18,"3"),
      button4(x + 5, y + 84, 18, 18,"4"),
      button5(x + 5, y +104, 18, 18,"5"),
      button6(x + 5, y +124, 18, 18,"6"),
      
      dial1(x+15, y +155, 24, 24, "A"),
      dial2(x+45, y +155, 24, 24, "B"),
      dial3(x+75, y +155, 24, 24, "C")
    {
      end(); // close the group
    }
    
    ~GTrack()
    {
      free(title);
    }
    
  
  private:
    char* title;
    
    Avtk::Background bg;
    
    Avtk::Button button1;
    Avtk::Button button2;
    Avtk::Button button3;
    Avtk::Button button4;
    Avtk::Button button5;
    Avtk::Button button6;
    
    Avtk::Dial dial1;
    Avtk::Dial dial2;
    Avtk::Dial dial3;
    
    
    static int privateID;
};

#endif // LUPPP_G_TRACK_H

