
#ifndef LUPPP_OPTIONS_H
#define LUPPP_OPTIONS_H

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>

#include "avtk/avtk_button.h"
#include "avtk/avtk_light_button.h"

class OptionsWindow
{
  public:
    OptionsWindow();
    
    void show();
    void setTarget(const char* t);
    void setBindEnable(bool b);
  
  private:
    Fl_Double_Window* window;
    Fl_Tabs*          tabs;
    
    std::string target;
    
    Avtk::Button* targetLabel;
    Avtk::Button* ctlrButton;
    Avtk::LightButton* bindEnable;
};

#endif // LUPPP_OPTIONS_H
