

#ifndef LUPPP_TRACKOUTPUTSTATE
#define LUPPP_TRACKOUTPUTSTATE

struct TrackOutputState
{
  int ID;
  
  // for parameter control
  bool selected;
  int selectedDevice;
  
  float volume;
  float pan;
  float panZ;
  
  bool mute, solo, recEnable;
};

#endif
