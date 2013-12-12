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

#include "audiobuffer.hxx"

#include <stdio.h>

AudioBuffer::AudioBuffer()
{
  ID = privateID++;
  init();
}

AudioBuffer::AudioBuffer(unsigned long size)
{
  // FIXME recorded buffers don't get an ID, using garbage IDs 
  /// no ID assigned: it *takes* the one from the previous buffer!
  init();
  buffer.resize(size);
}
    
void AudioBuffer::init()
{
  numBeats = 0;
  audioFrames = 0;
  memset( name, 0, sizeof(char)*20 );
  //sprintf( name, "%i", ID );
}

/// this function is used for "resizing" an exisiting buffer, and should
/// not be called for any other reason.
void AudioBuffer::setID(int id)
{
  ID = id;
}

int AudioBuffer::getID()
{
  return ID;
}

void AudioBuffer::setName(const char* n)
{
  memcpy( name, n, sizeof(char)* 19 );
  
  if ( strlen(n) > 19 )
  {
#ifdef DEBUG_BUFFER
    cout << "AudioBuffer setName too long, truncating." << endl;
#endif
    return;
  }
  
}

char* AudioBuffer::getName()
{
  return name;
}

int AudioBuffer::getBeats()
{
  return numBeats;
}

void AudioBuffer::setBeats(int b)
{
#ifdef DEBUG_BUFFER
  cout << "AudioBuffer now has " << b << " beats\n" << endl;
#endif
  numBeats = b;
}

void AudioBuffer::setAudioFrames(long af)
{
  audioFrames = af;
#ifdef DEBUG_BUFFER
  cout << "AudioBuffer " << ID << " has "  << audioFrames << " audioFrames\n" << endl;
#endif
}

long AudioBuffer::getAudioFrames()
{
  return audioFrames;
}

std::vector<float>& AudioBuffer::getData()
{
  return buffer;
}

void AudioBuffer::nonRtSetSample(std::vector<float>& sample)
{
  buffer.swap(sample);
}
void AudioBuffer::nonRtResize(unsigned long size)
{
  buffer.resize(size);
}

/*
ostream& AudioBuffer::operator<<(ostream& o, const AudioBuffer& a)
{
  o << "AudioBuffer " << a.name <<
        " beats: " << a.numBeats <<
        " audioFrames: " << a.audioFrames << endl;
  return o;
}
*/
