
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
class OptionsWindow;

/// contains UI elements to represent one controller
class ControllerUI
{
  public:
    ControllerUI( int x, int y, int w, int h, std::string name,int id);
    ~ControllerUI();
    
    void setTarget(const char* n);
    
    void setBindEnable( bool b );
    void addBinding( Binding* b );
    
    // the ControllerID this UI class represents
    int controllerID;
    
    // for adding to GOptions tabs
    Fl_Group* widget;
    
    char* name;
    
    OptionsWindow* optionsWindow;
  
  private:
    // bindings
    Avtk::Bindings*   bindings;
    Fl_Box* targetLabel;
    Fl_Box* targetLabelStat;
    Avtk::LightButton* bindEnable;
    Avtk::Button* writeControllerBtn;
    Avtk::Button* removeController;
    
    
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
    
    // public for static methods only
    Fl_Tabs* tabs;
    std::vector<ControllerUI*> controllers;
    Fl_Group* addGroup;
  
  private:
    Fl_Double_Window* window;
    Avtk::Button* newButton;
    Avtk::Button* loadButton;
};

#endif // LUPPP_OPTIONS_H
