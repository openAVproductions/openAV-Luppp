
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/apc.hxx"
#include "controller/controller.hxx"

using namespace std;

class ControllerUpdator
{
  public:
    ControllerUpdator()
    {
      c.push_back( AkaiAPC() );
    }
    
    void mute(int t, bool b)
    {
      for(int i = 0; i < c.size(); i++) c.at(i).mute(t,b);
    }
    
    void clipSelect(int t, bool cs)
    {
      for(int i = 0; i < c.size(); i++)
        c.at(i).clipSelect(t,cs);
    }
    
    void record(int t, bool r)
    {
      cout << "record() " << t << " " << r << endl;
      for(int i = 0; i < c.size(); i++)
        c.at(i).record(t,r);
    }
    
    void volume(int t, float v)
    {
      for(int i = 0; i < c.size(); i++) c.at(i).volume(t,v);
    }
  
  private:
    std::vector<Controller> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

