
#ifndef LUPPP_BACKTRACK
#define LUPPP_BACKTRACK

// This struct saves the primary state of the effects of a track, as well
// as the active status & send amount.

// Its purpose is to backup the track's current effect state, send state etc
// and then be able to restore that later when the user requests it.

struct BackTrack
{
  BackTrack()
  {
    for(int i = 0; i < 8; i++)
    {
      active[i] = false;
      values[i] = 0.f;
      sends[0]  = 0.f;
    }
  }
  
  bool  active[8];
  float values[8];
  float sends[1];
};

#endif

