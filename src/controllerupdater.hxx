
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/apc.hxx"
#include "controller/controller.hxx"

#include "gridlogic.hxx"

using namespace std;

// this is a wrapper class around a vector of Controller instances
// Call the method on this class, and all controllers will be updated
class ControllerUpdater
{
  public:
    ControllerUpdater() {}
    
    void registerController( Controller* controller )
    {
      std::cout << "ControllerUpdater registering controller: " <<
        controller->getName() << endl;
      c.push_back( controller );
    }
    
    
    void mute(int t, bool b)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->mute(t,b);
    }
    
    void setTrackSceneProgress(int t, int s, float p)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->progress(t,s,p);
    }
    
    void setSceneState(int t, int clip, GridLogic::State s)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->setSceneState(t,clip,s);
    }
    
    void recordArm(int t, bool r)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->recordArm(t,r);
    }
    
    void volume(int t, float v)
    {
      for(unsigned int i = 0; i < c.size(); i++) c.at(i)->volume(t,v);
    }
  
  private:
    std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

