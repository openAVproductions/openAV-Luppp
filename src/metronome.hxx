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

#ifndef LUPPP_METRONOME_H
#define LUPPP_METRONOME_H

#include <cmath>
#include <iostream>

#include "buffers.hxx"

#include "observer/time.hxx"

using namespace std;

class Metronome : public TimeObserver
{
  public:
    Metronome();
    ~Metronome(){};
    
    void setActive(bool a);
    
    void bar();
    void beat();
    void setFpb(int f);
    
    void process(int nframes, Buffers* buffers);
  
  private:
    int fpb;
    bool playBar;
    bool active;
    
    int playPoint, endPoint;
    float barSample[44100];
    float beatSample[44100];
  
};

#endif // LUPPP_METRONOME_H
