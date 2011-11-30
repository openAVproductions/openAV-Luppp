
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
};

class Effect
{
  public:
    Effect();
    virtual void process(int, float*);
  
  protected:
    
};

#endif

