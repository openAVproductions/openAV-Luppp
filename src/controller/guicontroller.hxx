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


#ifndef LUPPP_CONTROLLER_GUI_H
#define LUPPP_CONTROLLER_GUI_H

#include "controller.hxx"

#include "../observer/midi.hxx"

class LupppGUI : public Controller
{
  public:
    LupppGUI();
    
    std::string getName(){return "Luppp GUI";}
    
    void masterVolume(float f);
    void masterReturnVolume(float f);
    void masterInputVol(float v);
    void masterInputTo(int to,float f);
    void masterInputToActive(int to,float f);
    
    void metronomeEnable(bool b);
    
    void trackSend(int t, int send, float r);
    void trackSendActive(int t, int send, bool a);
    
    void bpm(int bpm);
    void tapTempo( bool b );
    
    void specialScene(int t, int scene);
    
    void mute(int t, bool b);
    void volume(int t, float f);
    void progress(int t, int s, float p);
    void recordArm(int t, bool b);
    void launchScene( int scene );
    void setSceneState(int track, int clip, GridLogic::State s);
    
};

#endif // LUPPP_CONTROLLER_GUI_H

