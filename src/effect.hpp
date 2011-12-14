
#ifndef LUPPP_EFFECT
#define LUPPP_EFFECT

#include <iostream>
#include <vector>

enum EffectType
{
  EFFECT_REVERB = 0,
  EFFECT_TRANSIENT,
  EFFECT_PARAMETRIC_EQ,
  EFFECT_COMPRESSOR,
  EFFECT_LOWPASS,
  EFFECT_HIGHPASS,
};

class Effect
{
  public:
    Effect(){ ID = privateID++; }
    int  getID(){ return ID; }
    virtual void process(int, float*){};
  
  private:
    int ID;
    static int privateID;
};

#endif

