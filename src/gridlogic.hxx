
#ifndef LUPPP_GRID_LOGIC_H
#define LUPPP_GRID_LOGIC_H

#include "config.hxx"
#include "observer/time.hxx"

/** GridLogic
 *  The logic code for the luppp tracks / grid resides here. This logic is
 *  separtated from the Looper class so it can be repurposed by different
 *  controllers and input devices. The UI and eg. APC / Launchpad all have a
 *  similar grid style interface: implementing the logic in each is not good.
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
      STATE_LOADED,
      STATE_STOP_QUEUED,
      STATE_RECORDING,
      STATE_RECORD_QUEUED,
    };
    
    GridLogic();
    
    /// button press / click event
    void pressed( int track, int scene );
    
    /// button release / click-release event
    void released( int track, int scene );
    
    /// resend entire grid state to controllers
    void updateState();
    
    /// resend track / scene combo to controllers
    void updateState( int track, int scene );
    
    
    /// time functions, not for use by Controller subclasses
    void bar();
    void beat();
  
  private:
    /// contains the current state of each grid square
    State state[NTRACKS*NSCENES];
    
    
};

#endif // LUPPP_GRID_LOGIC_H
