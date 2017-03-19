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


#include "looper.hxx"

#include "config.hxx"

#include <cmath>

#include "jack.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "controllerupdater.hxx"

extern Jack* jack;

Looper::Looper(int t) :
	AudioProcessor(),
	TimeObserver(),
	track(t)
{
	uiUpdateConstant= jack->getSamplerate() / 30.f;
	uiUpdateCounter = jack->getSamplerate() / 30.f;

	// pre-zero the internal sample
	//tmpRecordBuffer = (float*)malloc( sizeof(float) * MAX_BUFFER_SIZE );
	//memset( tmpRecordBuffer, 0, sizeof(float) * MAX_BUFFER_SIZE );

	for(int i = 0; i < 10; i++ ) {
		clips[i] = new LooperClip(track, i);
	}

	tmpBuffer.resize( MAX_BUFFER_SIZE );

	fpb = 22050;

	// init faust pitch shift variables
	fSamplingFreq = jack->getSamplerate();
	IOTA = 0;

	//tmpRecordBuffer.resize(MAX_BUFFER_SIZE);

	for (int i=0; i<65536; i++)
		fVec0[i] = 0;
	for (int i=0; i<2; i++)
		fRec0[i] = 0;
	semitoneShift = 0.0f;
	windowSize = 1000;
	crossfadeSize = 1000;
}

LooperClip* Looper::getClip(int scene)
{
	return clips[scene];
}

void Looper::beat()
{
	//TODO needed?
	//FIXME: Need to keep looperClips in sync when there exists no int N
	// such that playSpeed*N==1
//    for(int i=0;i<NSCENES;i++)
//    {
//        int iph=clips[i]->getPlayhead()+1.0;
//        long targetFrames = clips[i]->getBeats() * fpb;
//        long actualFrames = clips[i]->getActualAudioLength();//getBufferLenght();
//        float playSpeed = 1.0;

//        if ( targetFrames != 0 && actualFrames != 0 )
//        {
//          playSpeed = float(actualFrames) / targetFrames;
//        }
//        clips[i]->setPlayHead(iph-(iph%fpb)*playSpeed);

//    }

}

void Looper::setRequestedBuffer(int s, AudioBuffer* ab)
{
	clips[s]->setRequestedBuffer( ab );
}

void Looper::setFpb(int f)
{
	fpb = f;
}

void Looper::process(unsigned int nframes, Buffers* buffers)
{
	// process each clip individually: this allows for playback of one clip,
	// while another clip records.
	for ( int clip = 0; clip < NSCENES; clip++ ) {
		// handle state of clip, and do what needs doing:
		// record into buffer, play from buffer, etc
		if ( clips[clip]->recording() ) {
			if ( clips[clip]->recordSpaceAvailable() <  LOOPER_SAMPLES_BEFORE_REQUEST &&
			     !clips[clip]->newBufferInTransit() ) {
				EventLooperClipRequestBuffer e( track, clip, clips[clip]->audioBufferSize() + LOOPER_SAMPLES_UPDATE_SIZE);
				writeToGuiRingbuffer( &e );
				clips[clip]->newBufferInTransit(true);
			}

			// copy data from input buffer to recording buffer
			float* inputL = buffers->audio[Buffers::MASTER_INPUT_L];
			float* inputR = buffers->audio[Buffers::MASTER_INPUT_R];
			clips[clip]->record( nframes, inputL, inputR);
		} else if ( clips[clip]->playing() ) {
			// copy data into tmpBuffer, then pitch-stretch into track buffer
			long targetFrames = clips[clip]->getBeats() * fpb;
			long actualFrames = clips[clip]->getActualAudioLength();//getBufferLenght();
			float playSpeed = 1.0;

			if ( targetFrames != 0 && actualFrames != 0 ) {
				playSpeed = float(actualFrames) / targetFrames;
			}

			float* outL = buffers->audio[Buffers::SEND_TRACK_0_L + track];
			float* outR = buffers->audio[Buffers::SEND_TRACK_0_R + track];

			for(unsigned int i = 0; i < nframes; i++ ) {
				// REFACTOR into system that is better than per sample function calls
                float tmpL = 0;
                float tmpR = 0;
				clips[clip]->getSample(playSpeed, &tmpL, &tmpR);

				float deltaPitch = 12 * log ( playSpeed ) / log (2);
				semitoneShift = -deltaPitch;

				// write the pitch-shifted signal to the track buffer
				//FIXME: pitchShift adds delay even for playSpeed = 1.0!!
				//we should use something better (e.g librubberband)
				if(playSpeed!=1.0f) {
					pitchShift( 1, &tmpL, &outL[i] );
					pitchShift( 1, &tmpR, &outR[i] );
                }
				else {
					outL[i]+=tmpL;
					outR[i]+=tmpR;
                }
			}

			//printf("Looper %i playing(), speed = %f\n", track, playSpeed );

			if ( uiUpdateCounter > uiUpdateConstant ) {
				jack->getControllerUpdater()->setTrackSceneProgress(track, clip, clips[clip]->getProgress() );
				uiUpdateCounter = 0;
			}
			uiUpdateCounter += nframes;
		}

	}


}

void Looper::resetTimeState()
{
	for(int i=0; i<NSCENES; i++)
		clips[i]->setPlayHead(0.0);
}

void Looper::pitchShift(int count, float* input, float* output)
{
	float   fSlow0 = windowSize;
	float   fSlow1 = ((1 + fSlow0) - powf(2,(0.08333333333333333f * semitoneShift)));
	float   fSlow2 = (1.0f / crossfadeSize);
	float   fSlow3 = (fSlow0 - 1);
	float* input0 = &input[0];
	//float* output0 = &output[0];

	for (int i=0; i<count; i++) {
		float fTemp0 = (float)input0[i];
		fVec0[IOTA&65535] = fTemp0;
		fRec0[0] = fmodf((fRec0[1] + fSlow1),fSlow0);
		int iTemp1 = int(fRec0[0]);
		int iTemp2 = (1 + iTemp1);
		float fTemp3 = min((fSlow2 * fRec0[0]), 1.f );
		float fTemp4 = (fSlow0 + fRec0[0]);
		int iTemp5 = int(fTemp4);
		float out=output[0];
		out += (float)(((1 - fTemp3) * (((fTemp4 - iTemp5) *
		                                 fVec0[(IOTA-int((int((1 + iTemp5)) & 65535)))&65535]) + ((0 - ((
		                                                 fRec0[0] + fSlow3) - iTemp5)) * fVec0[(IOTA-int((iTemp5 & 65535)))
		                                                                 &65535]))) + (fTemp3 * (((fRec0[0] - iTemp1) * fVec0[(IOTA-int((int(
		                                                                                 iTemp2) & 65535)))&65535]) + ((iTemp2 - fRec0[0]) * fVec0[(IOTA-int((
		                                                                                                 iTemp1 & 65535)))&65535]))));

		output[0]=out;
		fRec0[1] = fRec0[0];
		IOTA = IOTA+1;
	}
}
