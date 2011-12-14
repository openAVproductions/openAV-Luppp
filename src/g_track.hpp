
#ifndef LUPPP_GTRACK
#define LUPPP_GTRACK

#include <vector>
#include <iostream>

#include <gtkmm.h>

#include "effect.hpp"

struct GEffectDetails
{
  EffectType type;
  
};

class GTrack
{
  public:
    GTrack();
    
    std::vector<Gtk::Widget*> widgetVector;
    
};

#endif
