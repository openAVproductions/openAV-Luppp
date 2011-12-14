
#ifndef LUPPP_EFFECT
#define LUPPP_EFFECT

#include <iostream>
#include <vector>

class Top;

enum EffectType
{
  EFFECT_REVERB = 0,
  EFFECT_TRANSIENT,
  EFFECT_PARAMETRIC_EQ,
  EFFECT_COMPRESSOR,
  EFFECT_LOWPASS,
  EFFECT_HIGHPASS,
  EFFECT_BEATSMASH
};

class Effect
{
  public:
    Effect(Top*, EffectType& t);
    
    int  getID(){ return ID; }
    EffectType getType(){return type;}
    
    virtual void process(int, float*){};
  
  private:
    int ID;
    static int privateID;
    EffectType type;
};

#endif

