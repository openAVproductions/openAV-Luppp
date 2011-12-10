

#ifndef LUPPP_TRACKOUTPUTSTATE
#define LUPPP_TRACKOUTPUTSTATE

struct TrackOutputState
{
  int ID;
  bool selected;
  float volume;
  float pan;
  float panZ;
  
  bool mute, solo, rec;
};

#endif
