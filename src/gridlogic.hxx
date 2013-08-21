
#ifndef LUPPP_GRID_LOGIC_H
#define LUPPP_GRID_LOGIC_H

#include "config.hxx"
#include "observer/time.hxx"

class AudioBuffer;

/** GridLogic
 *  The logic code for the luppp tracks / grid resides here. This logic is
 *  separtated from the Looper class so it can be repurposed by different
 *  controllers and input devices. The UI and eg. APC / Launchpad all have a
 *  similar grid style interface: the logic is implemented here once.
 * 
 *  The class sends Looper messages to change its state, thus abstracting away
 *  the details of the Looper control, and exposing its functionality using a
 *  convinient API.
 * 
 *  When the state of a block changes, it will update the state of the grid on
 *  the controller.
 * 
 *  It inherits from TimeObserver so it can change the state of the clip on the
 *  bar / beat.
**/
class GridLogic : public TimeObserver
{
  public:
    /// possible states of each square. Public so Controller subclasses can
    /// determine the state of the square
    enum State {
      STATE_EMPTY = 0,
      STATE_PLAYING,
      STATE_PLAY_QUEUED,
      STATE_STOPPED,
      STATE_STOP_QUEUED,
      STATE_RECORDING,
      STATE_RECORD_QUEUED,
    };
    
    GridLogic();
    
    /// button press / click event
    void pressed( int track, int scene );
    
    /// button release / click-release event
    void released( int track, int scene );
    
    /// GUI load event
    void load(int track, int scene, AudioBuffer* ab);
    
    /// resend entire grid state to controllers
    void updateState();
    
    /// time functions, not for use by Controller subclasses
    void bar();
    void beat();
  
    /// for debug purposes: use static_cast<int>(GridLogic::State) to access
    static const char* StateString[8];
    
};

#endif // LUPPP_GRID_LOGIC_H
