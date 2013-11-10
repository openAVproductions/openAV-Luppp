
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

/// contains UI elements to represent one controller
class ControllerUI
{
  public:
    ControllerUI( int x, int y, int w, int h, std::string name);
    ~ControllerUI();
    
    void setTarget(const char* n);
    
    void setBindEnable( bool b );
    void addBinding( Binding* b );
    
    // the ControllerID this UI class represents
    int controllerID;
  
  private:
    // bindings
    Avtk::Bindings*   bindings;
    Fl_Box* targetLabel;
    Fl_Box* targetLabelStat;
    Avtk::LightButton* bindEnable;
    Avtk::Button* writeControllerBtn;
    
    char* name;
    char* target;
    
    // Controller
    Avtk::Button* ctlrButton;
};

class OptionsWindow
{
  public:
    OptionsWindow();
    
    void show();
    void hide();
    
    ControllerUI* getControllerUI(int id);
  
  private:
    Fl_Double_Window* window;
    Fl_Tabs*          tabs;
    
    Avtk::Button* newButton;
    
    std::vector<ControllerUI> controllers;
    
};

#endif // LUPPP_OPTIONS_H
