

#ifndef LUPPP_APC_H
#define LUPPP_APC_H

#include "controller.hxx"

class AkaiAPC : public Controller
{
  public:
    AkaiAPC();
    
    void mute(int t, bool b);
    void clipSelect(int t, bool b);
    
    virtual void record(int t, bool b);
    void volume(int t, float f);
};

#endif // LUPPP_APC_H

