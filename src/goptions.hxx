
#ifndef LUPPP_OPTIONS_H
#define LUPPP_OPTIONS_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include "avtk/avtk_button.h"
#include "avtk/avtk_light_button.h"

class OptionsWindow
{
  public:
    OptionsWindow();
    
    void show();
    
    void setBindEnable(bool b);
  
  private:
    Fl_Double_Window* window;
    Avtk::Button* ctlrButton;
    Avtk::LightButton* bindEnable;
};

#endif // LUPPP_OPTIONS_H
