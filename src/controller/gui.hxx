

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
    
    void fxReverbSend(int t, float r);
    
    void mute(int t, bool b);
    void volume(int t, float f);
    void progress(int t, int s, float p);
    void recordArm(int t, bool b);
    void setSceneState(int track, int clip, GridLogic::State s);
    
};

#endif // LUPPP_CONTROLLER_GUI_H

