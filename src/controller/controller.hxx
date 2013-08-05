

#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

#include <string>

#include "../gridlogic.hxx"

class Controller
{
  public:
    Controller(){};
    virtual ~Controller(){};
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// master
    virtual void masterVolume(float f){};
    
    /// FX
    virtual void fxTrackSend(int t, int send, float v){};
    
    /// track functionality
    virtual void mute(int t, bool b){};
    virtual void volume(int t, float f){};
    virtual void progress(int t, int s, float f){};
    virtual void recordArm(int t, bool r){};
    virtual void setSceneState(int track, int scene, GridLogic::State s){};
};

#endif // LUPPP_CONTROLLER_H

