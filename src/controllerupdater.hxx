
#ifndef LUPPP_CONTROLLER_UPDATER_H
#define LUPPP_CONTROLLER_UPDATER_H

#include <vector>
#include <iostream>

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
    ControllerUpdater();
    
    void registerController( Controller* controller );
    
    // returns a Controller* by its name
    Controller* getController(std::string name);
    
    void reset();
    void mute(int t, bool b);
    
    void masterVolume(float v);
    
    void setTrackSceneProgress(int t, int s, float p);
    void setTrackSendActive(int t, int send, bool v);
    void setTrackSend(int t, int send, float v);
    
    void launchScene( int scene );
    
    void setSceneState(int t, int clip, GridLogic::State s);
    
    void recordArm(int t, bool r);
    
    void volume(int t, float v);
    
    void tapTempo(bool b);
    
    void metronomeEnable(bool b);
  
  private:
    std::vector<Controller*> c;
};

#endif // LUPPP_CONTROLLER_UPDATER_H

