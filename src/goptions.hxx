
#ifndef LUPPP_OPTIONS_H
#define LUPPP_OPTIONS_H

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>

#include "avtk/bindings.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_light_button.h"

class Binding;

class OptionsWindow
{
  public:
    OptionsWindow();
    
    void show();
    void setTarget(const char* t);
    void setBindEnable(bool b);
    
    void addBinding( Binding* b );
  
  private:
    Fl_Double_Window* window;
    Fl_Tabs*          tabs;
    
    // bindings
    Avtk::Bindings*   bindings;
    char* target;
    Fl_Box* targetLabel;
    Fl_Box* targetLabelStat;
    Avtk::LightButton* bindEnable;
    
    // Controller
    Avtk::Button* ctlrButton;
    
};

#endif // LUPPP_OPTIONS_H
