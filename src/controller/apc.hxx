

#ifndef LUPPP_APC_H
#define LUPPP_APC_H

#include "controller.hxx"

class AkaiAPC : public Controller
{
  public:
    AkaiAPC();
    
    void mute(int t, bool b);
    void volume(int t, float f);
    void recordArm(int t, bool b);
    void clipSelect(int track, int clip, ClipMode cm);
    
};

#endif // LUPPP_APC_H

