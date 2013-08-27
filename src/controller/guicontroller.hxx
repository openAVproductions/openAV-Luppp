

#ifndef LUPPP_CONTROLLER_GUI_H
#define LUPPP_CONTROLLER_GUI_H

#include "controller.hxx"

#include "../observer/midi.hxx"

class LupppGUI : public Controller
{
  public:
    LupppGUI();
    
    std::string getName(){return "Luppp GUI";}
    
    void masterVolume(float f);
    
    void trackSend(int t, int send, float r);
    
    void tapTempo( bool b );
    
    void mute(int t, bool b);
    void volume(int t, float f);
    void progress(int t, int s, float p);
    void recordArm(int t, bool b);
    void launchScene( int scene );
    void setSceneState(int track, int clip, GridLogic::State s);
    
};

#endif // LUPPP_CONTROLLER_GUI_H

