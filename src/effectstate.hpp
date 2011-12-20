
#ifndef LUPPP_EFFECTSTATE
#define LUPPP_EFFECTSTATE

struct EffectState
{
  EffectState(int id)
  {
    ID = id;
    for(int i = 0; i < 8; i++)
      values[i] = 0.f;
  }
  int ID;
  float values[8];
};

#endif

