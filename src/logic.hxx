
#ifndef LUPPP_LOGIC_H
#define LUPPP_LOGIC_H

#include "event.hxx"

/** Logic
 *  This class contains an interface exposing most functionality in Luppp. The
 *  interface is used to have one central place from where each controller can
 *  interact with Luppp using the same function calls.
 * 
 *  This class should be used for input from any device. The interface is
 *  deliberatly specific with regards to scheduling events: controllers should
 *  NOT attempt to schedule changes: use the provided functions directly when
 *  events occur on a controller.
 * 
 *  The ControllerUpdater class is the opposite of this class, it provides
 *  feedback of each event that occurs.
 * 
 *  Note: The GridLogic class provides the interface to scene selection / state.
**/
class Logic
{
  public:
    Logic();
    
    void tapTempo();
    
    void metronomeEnable(bool b);
    
    void masterInputVol( float v );
    void masterInputTo( Event::INPUT_TO inputTo, float v);
    void masterInputToActive( Event::INPUT_TO inputTo, bool active);
    
    void masterReturn(int t, int send, float v);
    
    void trackVolume(int track, float volume);
    void trackRecordArm(int track, bool armed);
    void trackSendActive(int track, int send, bool active);
    void trackSend(int track, int send, float value);
    
    void looperUseAsTempo(int track, int scene);
    void looperClipLenght(int track, int scene, int lenght);
};

#endif // LUPPP_LOGIC_H
