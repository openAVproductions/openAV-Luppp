
#ifndef LUPPP_EFFECTSTATE
#define LUPPP_EFFECTSTATE

struct EffectState
{
  EffectState(int id)
  {
    ID = id;
    active = false;
    for(int i = 0; i < 8; i++)
      values[i] = 0.f;
  }
  int ID;
  bool active;
  float values[8];
};

#endif

