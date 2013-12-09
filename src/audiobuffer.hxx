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

#ifndef LUPPP_AUDIOBUFFER_H
#define LUPPP_AUDIOBUFFER_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>

using namespace std;

/// AudioBuffer stores float samples in a big vector.
class AudioBuffer
{
  public:
    AudioBuffer()
    {
      ID = privateID++;
      init();
    }
    AudioBuffer(unsigned long size)
    {
      // FIXME recorded buffers don't get an ID, using garbage IDs 
      /// no ID assigned: it *takes* the one from the previous buffer!
      init();
      buffer.resize(size);
    }
    
    void init()
    {
      numBeats = 0;
      audioFrames = 0;
      memset( name, 0, sizeof(char)*20 );
      //sprintf( name, "%i", ID );
    }
    
    /// this function is used for "resizing" an exisiting buffer, and should
    /// not be called for any other reason.
    void setID(int id)
    {
      ID = id;
    }
    
    int getID()
    {
      return ID;
    }
    
    void setName(const char* n)
    {
      if ( strlen(n) > 19 )
      {
#ifdef DEBUG_BUFFER
        cout << "AudioBuffer setName too long!" << endl;
#endif
        return;
      }
      
      memcpy( name, n, sizeof(char)* 20 ); 
    }
    
    char* getName()
    {
      return name;
    }
    
    int getBeats()
    {
      return numBeats;
    }
    
    void setBeats(int b)
    {
#ifdef DEBUG_BUFFER
      cout << "AudioBuffer now has " << b << " beats\n" << endl;
#endif
      numBeats = b;
    }
    
    void setAudioFrames(long af)
    {
      audioFrames = af;
#ifdef DEBUG_BUFFER
      cout << "AudioBuffer " << ID << " has "  << audioFrames << " audioFrames\n" << endl;
#endif
    }
    
    long getAudioFrames()
    {
      return audioFrames;
    }
    
    std::vector<float>& getData()
    {
      return buffer;
    }
    
    void nonRtSetSample(std::vector<float>& sample)
    {
      buffer.swap(sample);
    }
    void nonRtResize(unsigned long size)
    {
        buffer.resize(size);
    }
    
    friend ostream& operator<<(ostream& o, const AudioBuffer& a)
    {
      o << "AudioBuffer " << a.name <<
            " beats: " << a.numBeats <<
            " audioFrames: " << a.audioFrames << endl;
      return o;
    }
  
  protected:
    static int privateID;
    int ID;
    
    int numBeats;
    
    /// holds the number of samples that are usable audio, as opposed to
    /// buffer.size(), which also has non-used space at the end.
    long audioFrames;
    
    char name[20];
    
    std::vector<float> buffer;
};

#endif

