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

#ifndef LUPPP_BUFFERS_H
#define LUPPP_BUFFERS_H

#include <cstring>
#include <jack/transport.h>

class Buffers
{
  public:
    
    enum BUFFER {
      // AUDIO
      MASTER_INPUT  = 0,
      
      MASTER_OUT_L,
      MASTER_OUT_R,
      
      JACK_SEND_OUT,
      JACK_MASTER_OUT_L,
      JACK_MASTER_OUT_R,
      JACK_SIDECHAIN_KEY,
      JACK_SIDECHAIN_SIGNAL,
      
      SEND,
      SIDECHAIN_KEY,
      SIDECHAIN_SIGNAL,
      
      MASTER_RETURN_L,
      MASTER_RETURN_R,
      
      HEADPHONES_OUT,
      
      // MIDI
      MASTER_MIDI_INPUT,
      
      // track buffers: they are the "working" buffers per track:
      // the end result is mixed into the master output, while each
      // stage along the way the amplitude etc can be analysed
      TRACK_0,
      TRACK_1,
      TRACK_2,
      TRACK_3,
      TRACK_4,
      TRACK_5,
      TRACK_6,
      TRACK_7,
      
      BUFFER_COUNT,
    };
    
    Buffers()
    {
      memset( audio, 0, sizeof(float*)*BUFFER_COUNT);
      memset( midi , 0, sizeof(void *)*BUFFER_COUNT);
    }
    float* audio[BUFFER_COUNT];
    void*  midi [BUFFER_COUNT];
    
    // Jack details
    jack_nframes_t         nframes;
    jack_nframes_t         samplerate;
    
    jack_nframes_t          transportFrame;
    jack_position_t*        transportPosition;
    jack_transport_state_t* transportState;
    
};

#endif // LUPPP_BUFFERS_H

