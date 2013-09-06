

#ifndef LUPPP_GENERIC_MIDI_H
#define LUPPP_GENERIC_MIDI_H

#include "controller.hxx"

#include "../observer/midi.hxx"

class GenericMIDI : public Controller, public MidiObserver
{
  public:
    GenericMIDI();
    
    std::string getName(){return "Generic MIDI controller";}
    
    /// track actions
    //void mute(int t, bool b);
    
    
    void volume(int t, float f);
    /*
    void progress(int t, int s, float f);
    void recordArm(int t, bool b);
    void launchScene( int scene );
    void setSceneState(int track, int clip, GridLogic::State s);
    
    /// track FX
    void trackSend(int t, int send, float v);
    */
    
    void reset();
    
    void midi(unsigned char* data);
    
  private:
    int _port;
    /*
    bool shiftPressed;
    
    int footpedalTrack;
    int footpedalScene;
    
    /// for handling events
    void ccChange( int track, int cc, float value );
    void noteOff( int track, int note, int vel );
    void noteOn( int track, int note, int vel );
    */
};

#endif // LUPPP_APC_H

