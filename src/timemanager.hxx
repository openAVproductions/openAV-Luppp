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

#ifndef LUPPP_TIME_H
#define LUPPP_TIME_H

#include <iostream>
#include <cstdio>

#include "buffers.hxx"
#include "transport.hxx"
#include "eventhandler.hxx"

#include "observer/time.hxx"

using namespace std;

class TimeManager
{
  public:
    TimeManager();
    
    int getFpb();
    void setBpm(float bpm);
    void setBpmZeroOne(float bpm);
    void setFpb(float f);
    
    /// add a component to be updated for time events
    void registerObserver(TimeObserver* o);
    
    /// call this when a tempo-tap occurs
    void tap();
    
    /// called to process buffers->nframes samples. If a beat is present, this
    /// is handled gracefully, first calling process up to the beat, then doing
    /// a beat() event on all TimeObservers, and processing the remaining samples
    void process(Buffers* buffers);
    
    /// returns the number of samples till beat if a beat exists in this process
    /// Otherwise returns nframes
    int getNframesToBeat();
    
    /// TRANSPORT_STATE is defined in transport.hxx
    void setTransportState( TRANSPORT_STATE s );
  
  private:
    int samplerate;
    
    /// the "state" of the transport: rolling or stopped
    TRANSPORT_STATE transportState;
    
    /// number of frames per beat
    int fpb;
    
    /// holds the number of frames processed
    long long totalFrameCounter;
    
    /// frame number of the last beat
    long previousBeat;
    
    long beatFrameCountdown;
    
    /// counts bars / beats processed
    int barCounter;
    int beatCounter;
    
    int tapTempoPos;
    long long tapTempo[3];
    
    std::vector<TimeObserver*> observers;
};

#endif // LUPPP_TIME_H
