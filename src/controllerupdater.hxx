
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/apc.hxx"
#include "controller/controller.hxx"

using namespace std;

// this is a wrapper class around a vector of Controller instances
// Call the method on this class, and all controllers will be updated
class ControllerUpdater
{
  public:
    ControllerUpdater()
    {
      c.push_back( new AkaiAPC() );
    }
    
    void mute(int t, bool b)
    {
      for(int i = 0; i < c.size(); i++) c.at(i)->mute(t,b);
    }
    
    void clipSelect(int t, int clip, Controller::ClipMode cm)
    {
      for(int i = 0; i < c.size(); i++)
        c.at(i)->clipSelect(t,clip,cm);
    }
    
    void recordArm(int t, bool r)
    {
      for(int i = 0; i < c.size(); i++)
        c.at(i)->recordArm(t,r);
    }
    
    void volume(int t, float v)
    {
      for(int i = 0; i < c.size(); i++) c.at(i)->volume(t,v);
    }
  
  private:
    std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

