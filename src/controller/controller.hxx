

#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

class Controller
{
  public:
    /*
    make a state class for a whole track
    class TrackState {
      int track;
      
    };
    */
    
    enum ClipMode {
      CLIP_MODE_EMPTY,
      CLIP_MODE_PLAYING,
      CLIP_MODE_PLAY_QUEUED,
      CLIP_MODE_LOADED,
      CLIP_MODE_STOP_QUEUED,
      CLIP_MODE_RECORDING,
      CLIP_MODE_RECORD_QUEUED,
    };
    
    Controller(){};
    virtual ~Controller(){};
    
    //virtual void setTrack(TrackState& t);
    
    virtual void mute(int t, bool b){};
    virtual void volume(int t, float f){};
    virtual void recordArm(int t, bool r){};
    virtual void clipSelect(int track, int clip, ClipMode cm){};
};

#endif // LUPPP_CONTROLLER_H

