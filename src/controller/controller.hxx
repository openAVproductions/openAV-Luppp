

#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

#include <string>

#include "../gridlogic.hxx"

/** Controller
 *  This is the base class for any device that interacts with Luppp.
 *  Overriding the virtual functions allows the subclass to be notified about
 *  that event type. To provide input, #include gridlogic.hxx and logic.hxx
**/
class Controller
{
  public:
    /// used to report the controller status
    enum STATUS {
      CONTROLLER_ERROR = 0,
      CONTROLLER_OK,
    };
    
    /// creates the controller instance: this is called in the non-RT thread
    Controller();
    
    /// controllers have a unique ID
    int getID();
    
    /// allows instance to register MIDI observer type functionality
    virtual int registerComponents(){return 0;}
    
    virtual ~Controller(){};
    
    /// get the status of the controller
    virtual STATUS status(){return CONTROLLER_OK;}
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// master
    virtual void masterInputVol(float f){}
    virtual void masterInputTo(int to,float f){}
    virtual void masterInputToActive(int to,float f){}
    virtual void masterVolume(float f){}
    virtual void masterReturnVolume(float f){}
    virtual void metronomeEnable(bool b){}
    
    /// FX
    virtual void trackSend(int t, int send, float v){}
    virtual void trackSendActive(int t, int send, bool a){}
    
    /// Time
    virtual void bpm(int bpm){}
    virtual void tapTempo(bool b){}
    
    /// Special
    virtual void specialScene(int t, int scene){}
    
    /// track functionality
    virtual void mute(int t, bool b){}
    virtual void volume(int t, float f){}
    virtual void progress(int t, int s, float f){}
    virtual void recordArm(int t, bool r){}
    virtual void setSceneState(int track, int scene, GridLogic::State s){}
    virtual void launchScene( int scene ){}
    
    /// reset controller
    virtual void reset();
  
  private:
    static int privateID;
    int ID;
};

#endif // LUPPP_CONTROLLER_H

