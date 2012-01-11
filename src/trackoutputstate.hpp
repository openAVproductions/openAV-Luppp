

#ifndef LUPPP_TRACKOUTPUTSTATE
#define LUPPP_TRACKOUTPUTSTATE

struct TrackOutputState
{
  TrackOutputState()
  {
    ID = -1;
    selected = -1;
    selectedDevice = 0;
    
    volume = 0.f;
    pan    = 0.f;
    panZ   = 0.f;
    
    rms = 0.f;
    falloff = 0.f;
    
    mute = solo = recEnable = false;
  }
  
  int ID;
  
  // for parameter control
  bool selected;
  int selectedDevice;
  
  float volume;
  float pan;
  float panZ;
  
  float rms;
  float falloff;
  
  bool mute, solo, recEnable;
};

#endif
