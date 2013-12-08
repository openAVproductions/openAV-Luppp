/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    ~GridLogic(){};
    
    /// button press / click event
    void pressed( int track, int scene );
    
    /// button release / click-release event
    void released( int track, int scene );
    
    /// master controls, launches a horizontal scene with one event
    void launchScene( int scene );
    int  getLaunchedScene();
    
    /// selected track functions
    void setSelectTrackScene(bool b);
    void setSelectedTrack(int t);
    void setSelectedScene(int s);
    int getSelectedTrack();
    int getSelectedScene();
    
    void selectedTrackSceneEvent(bool pressed);
    
    void specialScene(int t, int s);
    
    /// GUI load event
    void load(int track, int scene, AudioBuffer* ab);
    
    /// resend entire grid state to controllers
    void updateState();
    
    /// time functions, not for use by Controller subclasses
    void bar();
    void beat();
  
    /// for debug purposes: use static_cast<int>(GridLogic::State) to access
    static const char* StateString[8];
    
#ifdef BUILD_TESTS
    int runTests();
#endif

  private:
    /// holds last scene launch
    int sceneLaunch;
    
    /// holds selected track / scene
    bool sampleTrackScene; // turn on to have selected clip, press event acted on
    int selectedTrack;
    int selectedScene;
};



#endif // LUPPP_GRID_LOGIC_H
