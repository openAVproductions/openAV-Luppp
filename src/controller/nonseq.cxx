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


#include "nonseq.hxx"

#include <string>

#include "../jack.hxx"
#include "../looper.hxx"
#include "../looperclip.hxx"

NonSeq::NonSeq() :
	Controller(),
	MidiIO()
{
	std::string name = "non-seq";
	registerMidiPorts( name );
}

void NonSeq::launchScene( int scene )
{
	unsigned char data[3];
	data[0] = 176;
	data[1] = 20;
	data[2] = scene;

	//LUPPP_NOTE("NonSeq::launchScene() %i, %i, %i\n", data[0],data[1],data[2] );
	writeMidi( data, 3 );
}

std::string NonSeq::getName()
{
	return "non-seq";
}


void NonSeq::setSceneState(int track, int scene, GridLogic::State s)
{
	unsigned char data[3];
	data[0] = 176;
	data[1] = 22; // off

	// check *actual* value of playing: *NOT* GridState::s, because it could be queued
	// for something else, but we want the *actual* here, not "queued". This is a unique
	// use case because were trying to control non-seq as if it were part of Luppp.
	if( jack->getLooper( track )->getClip( scene )->playing() )
		data[1] = 21;

	data[2] = track;

	//LUPPP_NOTE("NonSeq::setSceneState() %i, %i, %i\n", data[0],data[1],data[2] );
	writeMidi( data, 3 );
}


int NonSeq::registerComponents()
{
	// makes JACK add this controller to the midiObservers list:
	// note the static_cast<>() is *needed* here for multiple-inheritance
	MidiIO* m = static_cast<MidiIO*>(this);

	jack->registerMidiIO( m );

	return LUPPP_RETURN_OK;
}
