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


#ifndef LUPPP_NON_SEQ_H
#define LUPPP_NON_SEQ_H

#include "../observer/midi.hxx"
#include "controller.hxx"

/** NonSeq
 *  A class for integrating the NonSeq sequencer with Luppp, providing scene
 *  on / off MIDI events to NonSeq
**/
class NonSeq : public Controller, public MidiIO
{
  public:
    NonSeq();
    ~NonSeq(){};
    
    std::string getName();
    
    int registerComponents();
    
    void launchScene( int scene );
    
    void setSceneState(int track, int scene, GridLogic::State s);
};

#endif // LUPPP_NON_SEQ_H
