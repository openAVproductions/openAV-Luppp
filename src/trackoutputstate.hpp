

#ifndef LUPPP_TRACKOUTPUTSTATE
#define LUPPP_TRACKOUTPUTSTATE

struct TrackOutputState
{
  int ID;
  bool selected;
  float volume;
  float pan;
  
  bool mute, solo, rec;
};

#endif
