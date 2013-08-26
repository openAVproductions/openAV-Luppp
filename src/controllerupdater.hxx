
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

#include "controller/apc.hxx"
#include "controller/controller.hxx"

#include "gridlogic.hxx"

using namespace std;


/** ControllerUpdater
 *  Updates each registered controller when a certain event occurs. All output
 *  devices (MIDI controllers, GUI, OSC-UI's etc) are registered in order to
 *  stay up-to-date.
 * 
 *  This class does no scheduling, it passes the events to the Controllers
 *  immidiatly.
 * 
 *  The Logic class is the opposite of this: it takes input and Luppp processes
 *  it, pushing the relevant updates in state through ControllerUpdater to each
 *  registered device.
**/
class ControllerUpdater
{
  public:
    ControllerUpdater(){}
    
    void registerController( Controller* controller )
    {
      c.push_back( controller );
    }
    
    void reset()
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->reset();
    }
    
    void mute(int t, bool b)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->mute(t,b);
    }
    
    void masterVolume(float v)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->masterVolume(v);
    }
    
    void setTrackSceneProgress(int t, int s, float p)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->progress(t,s,p);
    }
    
    void setTrackSend(int t, int send, float v)
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->trackSend(t, send, v);
    }
    
    void launchScene( int scene )
    {
      for(unsigned int i = 0; i < c.size(); i++)
        c.at(i)->launchScene(scene);
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
    
    void tapTempo(bool b)
    {
      for(unsigned int i = 0; i < c.size(); i++) c.at(i)->tapTempo(b);
    }
  
  private:
    std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

