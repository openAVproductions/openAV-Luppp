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


#include "../gridlogic.hxx"

#include "../jack.hxx"
#include "../looperclip.hxx"


extern Jack* jack;

#ifdef BUILD_TESTS

#include "../config.hxx"
#include "qunit.hxx"

int GridLogic::runTests()
{
	QUnit::UnitTest qunit( QUnit::normal, true );
	int t = 0;
	int s = 0;
	LooperClip* lc  = jack->getLooper( t )->getClip( s );

	// "pretty" prints the state of the clip
	//LUPPP_NOTE("%s", GridLogic::StateString[ lc->getState() ] );

	/// SCENE LAUNCH
	lc->init();
	jack->getGridLogic()->launchScene( s );
	QUNIT_IS_TRUE( jack->getGridLogic()->getLaunchedScene() == s );

	/// QUEUE s1, then launch s2, s1
	int launchScene = s + 1;
	lc->init();
	lc->setState( true, false, false, true, false, false ); // loaded + qplay
	LUPPP_NOTE("state before = %s", GridLogic::StateString[ lc->getState() ] );
	jack->getGridLogic()->launchScene( launchScene ); // launch different clip

	QUNIT_IS_TRUE( jack->getGridLogic()->getLaunchedScene() == launchScene );
	LUPPP_NOTE("state after launch before bar = %s", GridLogic::StateString[ lc->getState() ] );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );

	jack->getGridLogic()->bar();

	LUPPP_NOTE("state after bar = %s", GridLogic::StateString[ lc->getState() ] );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );

	/// s1 playing, then launch s2, s1
	lc->init();
	lc->setState( true, true, false, false, false, false ); // playing
	//LUPPP_NOTE("state before = %s", GridLogic::StateString[ lc->getState() ] );
	jack->getGridLogic()->launchScene( launchScene ); // launch different clip
	QUNIT_IS_TRUE( jack->getGridLogic()->getLaunchedScene() == launchScene );
	//LUPPP_NOTE("state after before bar = %s", GridLogic::StateString[ lc->getState() ] );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
	jack->getGridLogic()->bar();
	//LUPPP_NOTE("state after bar = %s", GridLogic::StateString[ lc->getState() ] );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );


	/// PRESS PAD
	// empty -> recording
	lc->init();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_EMPTY );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORD_QUEUED );
	jack->getGridLogic()->bar();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORDING );

	float buffer[64];
	lc->record(64, &buffer[0], &buffer[0]);

	// recording -> playing
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
	jack->getGridLogic()->bar();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );
	// playing -> stopped
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
	jack->getGridLogic()->bar();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );
	// stopped -> playing
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
	jack->getGridLogic()->bar();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );


	/// DOUBLE PRESS PAD
	// empty -> recordQ -> empty
	lc->init();
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_EMPTY );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORD_QUEUED );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_EMPTY );
	lc->bar();

	// recording -> playing -> stopped
	lc->setState( true, false, true, false, false, false );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_RECORDING );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
	lc->bar();

	// stopped -> playing -> stopped
	lc->setState( true, false, false, false, false, false );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOPPED );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAY_QUEUED );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );

	// stopped -> playing
	lc->setState( true, true, false, false, false, false );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_STOP_QUEUED );
	jack->getGridLogic()->pressed( t, s );
	jack->getGridLogic()->released( t, s );
	QUNIT_IS_TRUE( lc->getState() == GridLogic::STATE_PLAYING );
	return qunit.errors();
}

#endif // BUILD_TESTS

