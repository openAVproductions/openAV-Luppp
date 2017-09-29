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

#include "gridlogic.hxx"

#include "jack.hxx"

#include "trackoutput.hxx"
#include "controllerupdater.hxx"

extern Jack* jack;

const char* GridLogic::StateString[8] = {
	"Empty",
	"Playing",
	"Play queued",
	"Stopped",
	"Stop queued",
	"Recording",
	"Record queued"
};

GridLogic::GridLogic()
{
	sceneLaunch = 0;

	sampleTrackScene = false;
	selectedTrack = 0;
	selectedScene = 0;
}

void GridLogic::selectedTrackSceneEvent(bool p)
{
	if ( p ) {
		pressed( selectedTrack, selectedScene );
	} else {
		released( selectedTrack, selectedScene );
	}
}

void GridLogic::setSelectTrackScene(bool b)
{
	char tmp[40];
	sprintf( tmp, "Select track enable %i", int(b) );
	EventGuiPrint e( tmp );
	writeToGuiRingbuffer( &e );
	sampleTrackScene = b;
}

int GridLogic::getLaunchedScene()
{
	return sceneLaunch;
}

int GridLogic::getSelectedTrack()
{
	return selectedTrack;
}

int GridLogic::getSelectedScene()
{
	return selectedScene;
}

void GridLogic::setSelectedTrack(int t)
{
	selectedTrack = t;
}

void GridLogic::setSelectedScene(int s)
{
	selectedScene = s;
}


void GridLogic::launchScene( int scene )
{
	for(unsigned int t = 0; t < NTRACKS; t++ ) {
		for(int s = 0; s < NSCENES; s++ ) {
			LooperClip* lc = jack->getLooper( t )->getClip( s );
			if ( s == scene ) {
				lc->queuePlay();
				jack->getControllerUpdater()->setSceneState( t, s, lc->getState() );
			} else {
				if ( lc->playing() ) {
					lc->queueStop();
					jack->getControllerUpdater()->setSceneState( t, s, lc->getState() );
				} else if ( lc->somethingQueued() ) {
					lc->neutralize();
					jack->getControllerUpdater()->setSceneState( t, s, lc->getState() );
				}
			}
		}
	}

	sceneLaunch = scene;

	jack->getControllerUpdater()->launchScene( scene );
}

void GridLogic::specialScene(int t, int s)
{
	if ( t < 0 ) t = 0;
	if ( t >= NTRACKS ) t = NTRACKS-1;

	if ( s < 0 ) s = 0;
	if ( s >= NSCENES ) s = NSCENES-1;

	selectedTrack = t;
	selectedScene = s;

	// update UI's
	jack->getControllerUpdater()->specialScene( t, s );
}

void GridLogic::pressed( int track, int scene )
{
	if ( sampleTrackScene ) {
		specialScene( track, scene );

		// don't act on grid press!
		return;
	}


	// get the clip, do the "press" action based on current state.
	LooperClip* lc  = jack->getLooper( track )->getClip( scene );
	TrackOutput* to = jack->getTrackOutput( track );
	GridLogic::State s = lc->getState();

#ifdef DEBUG_CLIP
	printf("GridLogic::pressed() before press state = %s\n", StateString[ int(scene) ] );
#endif

	if ( to->recordArm() && !lc->recording() ) {
		lc->queueRecord();
		to->recordArm(false);
		jack->getControllerUpdater()->recordArm( track, false );
	} else {
		if ( s == STATE_EMPTY )
			lc->queueRecord();

		if ( s == STATE_STOPPED ) {
			// hack, stop all scenes, then launch proper one
			for( int i = 0; i < NTRACKS; i++ ) {
				LooperClip* ilc = jack->getLooper( track )->getClip( i );
				if ( ilc->playing() ) {
					ilc->queueStop();
				}
			}

			lc->queuePlay();
		}

		if ( s == STATE_PLAYING )
			lc->queueStop();

		if ( s == STATE_RECORDING )
			lc->queuePlay();

		if ( s == STATE_PLAY_QUEUED )
			lc->queueStop();

		if ( s == STATE_STOP_QUEUED )
			lc->queuePlay();

		// don't re-trigger if already playing!
		if ( s == STATE_STOP_QUEUED && lc->playing() )
			lc->neutralize();

		if ( s == STATE_RECORD_QUEUED )
			lc->neutralize();
	}

	// check state of new clip, if getQueuePlay() == true, queueStop() all other scenes
	//if ( lc->getQueuePlay() )
	{
		for(int i = 0; i < NSCENES; i++) {
			// exclude current scene
			if ( i != scene ) {
				//LUPPP_NOTE("netralizing & qStop on scene %i due to press on %i", i, scene );
				LooperClip* ilc = jack->getLooper( track )->getClip( i );

				ilc->neutralize();
				ilc->queueStop();
				jack->getControllerUpdater()->setSceneState(track, i, ilc->getState() );
			}
		}
	}


	s = lc->getState();
#ifdef DEBUG_CLIP
	printf("GridLogic::pressed() after press state = %s\n", StateString[ int(s) ] );
#endif
	jack->getControllerUpdater()->setSceneState(track, scene, s );
}

void GridLogic::clear( int track, int scene )
{
	jack->getLooper( track )->getClip( scene )->init();
	jack->getControllerUpdater()->setTrackSceneProgress(track, scene, 0 );
	jack->getControllerUpdater()->setSceneState( track, scene, GridLogic::STATE_EMPTY );
}

void GridLogic::released( int track, int scene )
{
	GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
	jack->getControllerUpdater()->setSceneState(track, scene, s );
}

void GridLogic::load(int track, int scene, AudioBuffer* ab)
{
	jack->getLooper( track )->getClip( scene )->load( ab );
	GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
	jack->getControllerUpdater()->setSceneState(track, scene, s );
}


void GridLogic::updateState()
{
	//printf("GridLogic::updateState() stub" );
	for(int t = 0; t < NTRACKS; t++) {
		for(int s = 0; s < NSCENES; s++) {
			GridLogic::State st = jack->getLooper( t )->getClip( s )->getState();
			EventGuiPrint e( GridLogic::StateString[st] );
			writeToGuiRingbuffer( &e );
			jack->getControllerUpdater()->setSceneState(t, s, st );
		}
	}
}

void GridLogic::bar()
{
#ifdef DEBUG_CLIP
	EventGuiPrint e( "GridLogic::bar()" );
	//writeToGuiRingbuffer( &e );
#endif

	/// iterate over all clips, if they're set to QUEUED, set to the next state
	for( int i = 0; i < NTRACKS*NSCENES; i++ ) {
		int track = i / NSCENES;
		int scene = i - track * NSCENES;
		jack->getLooper( track )->getClip(scene)->bar();

#ifdef DEBUG_CLIP
		GridLogic::State s = jack->getLooper( track )->getClip( scene )->getState();
		if ( s != STATE_EMPTY ) {
			//printf("%i, %i:after bar() state = %s\n", track, scene, StateString[ int(s) ] );
		}
#endif
	}
}


void GridLogic::beat()
{


}

