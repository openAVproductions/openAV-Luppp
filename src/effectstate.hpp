
#ifndef LUPPP_EFFECTSTATE
#define LUPPP_EFFECTSTATE

struct EffectState
{
  EffectState(int i) {ID = i; }
  int ID;
  float values[8];
};

#endif

