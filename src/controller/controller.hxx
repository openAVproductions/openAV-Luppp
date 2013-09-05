

#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

#include <string>

#include "../gridlogic.hxx"

class Controller
{
  public:
    /// registers subclass instance with ControllerUpdater
    Controller();
    
    virtual ~Controller(){};
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// master
    virtual void masterVolume(float f){};
    
    /// FX
    virtual void trackSend(int t, int send, float v){};
    
    /// Time
    virtual void bpm(int bpm){};
    virtual void tapTempo(bool b){};
    
    /// track functionality
    virtual void mute(int t, bool b){};
    virtual void volume(int t, float f){};
    virtual void progress(int t, int s, float f){};
    virtual void recordArm(int t, bool r){};
    virtual void setSceneState(int track, int scene, GridLogic::State s){};
    virtual void launchScene( int scene ){};
    
    /// reset controller
    virtual void reset();
};

#endif // LUPPP_CONTROLLER_H

