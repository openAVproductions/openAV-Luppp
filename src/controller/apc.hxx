

#ifndef LUPPP_APC_H
#define LUPPP_APC_H

#include "controller.hxx"

#include "../observer/midi.hxx"

class AkaiAPC : public Controller, public MidiObserver
{
  public:
    AkaiAPC();
    
    std::string getName(){return "Akai APC";}
    
    /// track actions
    void mute(int t, bool b);
    void volume(int t, float f);
    void progress(int t, int s, float f);
    void recordArm(int t, bool b);
    void launchScene( int scene );
    void setSceneState(int track, int clip, GridLogic::State s);
    
    /// track FX
    void fxTrackSend(int t, int send, float v);
    
    void midi(unsigned char* data);
    
  private:
    /// for handling events
    void ccChange( int track, int cc, float value );
    void noteOff( int track, int note, int vel );
    void noteOn( int track, int note, int vel );
};

#endif // LUPPP_APC_H

