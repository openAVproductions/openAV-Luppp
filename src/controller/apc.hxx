

#ifndef LUPPP_APC_H
#define LUPPP_APC_H

#include "controller.hxx"

#include "../observer/midi.hxx"

class AkaiAPC : public Controller, public MidiObserver
{
  public:
    AkaiAPC();
    
    void mute(int t, bool b);
    void volume(int t, float f);
    void recordArm(int t, bool b);
    void clipSelect(int track, int clip, ClipMode cm);
    
    void midi(unsigned char* data);
    
};

#endif // LUPPP_APC_H

