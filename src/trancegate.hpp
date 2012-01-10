
#ifndef LUPPP_TRANCEGATE
#define LUPPP_TRANCEGATE

#include <iostream>
#include <deque>

class Top;
#include "effect.hpp"

class TranceGate : public Effect
{
  public:
    TranceGate(Top*, EffectType);
    
    void process(int nframes, float *L);
  
  protected:
    enum GateState {
      GATE_OFF = 0,
      GATE_ATTACK,
      GATE_HOLD,
      GATE_DECAY,
      GATE_RELEASE,
      GATE_SUSTAIN
    };
    
    
    int oldBeat;
    int gate;
    
    int gateAttack;
    
    int ID;
    Top* top;
    
    int active;
    
    int nframeBeatCounter;
    
    // like a vector, but access from front & back is equally fast
    std::deque<float> audioBuffer;
    
};

#endif

