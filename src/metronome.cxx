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

#include "metronome.hxx"

#include <cmath>
#include <iostream>

#include "jack.hxx"
#include "buffers.hxx"
#include "observer/time.hxx"

extern Jack* jack;

using namespace std;

Metronome::Metronome() :
	TimeObserver(),
	playBar    (false),
	active     (false),
	playPoint  (0)
{
	//Create Beat/Bar samples
	beatSample=new float[jack->getSamplerate()];
	barSample=new float[jack->getSamplerate()];
	// create beat and bar samples
	endPoint = ( jack->getSamplerate()/10 );
	// samples per cycle of
	float scale = 2 * 3.1415 *880/jack->getSamplerate();

	// And fill it up
	for(int i=0; i < jack->getSamplerate(); i++) {
		beatSample[i]= sin(i*scale);
		barSample [i]= sin(i*scale*2);
	}

	setVolume(0.5);

	// don't play after creation
	playPoint = endPoint + 1;
}

Metronome::~Metronome()
{
	if(beatSample)
		delete [] beatSample;
	if(barSample)
		delete [] barSample;
}

void Metronome::setActive(bool a)
{
	active = a;
	// don't play immidiatly
	playPoint = endPoint + 1;
}

void Metronome::setVolume( float v )
{
	vol = v * 0.5;
	//printf(" vol = %f \n", vol );
}

void Metronome::bar()
{
	playPoint = 0;
	playBar = true;
}

void Metronome::beat()
{
	playPoint = 0;
	playBar = false;
}

void Metronome::setFpb(double f)
{
	fpb = f;

	// disable play until next beat
	playPoint = endPoint + 1;
}

void Metronome::process(int nframes, Buffers* buffers)
{
	if ( not active )
		return;

	float* outL = buffers->audio[Buffers::HEADPHONES_OUT_L];
	float* outR = buffers->audio[Buffers::HEADPHONES_OUT_R];

	float* sample = &beatSample[0];
	if( playBar ) {
		sample = &barSample[0];
	}

	for(int i = 0; i < nframes; i++) {
		if ( playPoint < endPoint ) {
			outL[i] += sample[playPoint] * vol;
			outR[i] += sample[playPoint] * vol;
			playPoint++;
		}
	}
}

