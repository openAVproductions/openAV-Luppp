
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/apc.hxx"
#include "controller/controller.hxx"

using namespace std;

// this is a wrapper class around a vector of Controller instances
// Call the method on this class, and all controllers will be updated
class ControllerUpdator
{
  public:
    enum ClipMode {
      CLIP_MODE_PLAYING,
      CLIP_MODE_PLAY_QUEUED,
      CLIP_MODE_LOADED,
      CLIP_MODE_EMPTY,
    };
    
    
    ControllerUpdator()
    {
      c.push_back( new AkaiAPC() );
    }
    
    void mute(int t, bool b)
    {
      for(int i = 0; i < c.size(); i++) c.at(i)->mute(t,b);
    }
    
    void clipSelect(int t, ClipMode cm)
    {
      for(int i = 0; i < c.size(); i++)
        c.at(i)->clipSelect(t,cm);
    }
    
    void record(int t, bool r)
    {
      cout << "record() " << t << " " << r << endl;
      for(int i = 0; i < c.size(); i++)
        c.at(i)->record(t,r);
    }
    
    void volume(int t, float v)
    {
      for(int i = 0; i < c.size(); i++) c.at(i)->volume(t,v);
    }
  
  private:
    std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

