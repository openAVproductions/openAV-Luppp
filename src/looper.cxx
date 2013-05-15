

#include "looper.hxx"

#include "jack.hxx"

void Looper::setState(State s)
{
  // before update, check if we recording, if so, print info
  if ( state == STATE_RECORDING )
  {
    int newBpm = lastWrittenSampleIndex / 44100;
    cout << "Looper " << track << " ending record: endPoint @ " << lastWrittenSampleIndex
         << ". Bpm " << newBpm << " perhaps?" << endl;
    
    jack->getTimeManager()->setBpm( newBpm );
  }
  
  // quantize?!
  state = s;
  
  if ( state == STATE_PLAYING ) // setup PLAY
  {
    endPoint = lastWrittenSampleIndex;
    playPoint = 0;
    //cout << "State = PLAYING, endPoint = " << endPoint << endl;
  }
  else if ( state == STATE_RECORDING ) // setup REC
  {
    //cout << "State = RECORDING" << endl;
    playPoint = 0;
    endPoint = 0;
    lastWrittenSampleIndex = 0;
  }
  else if ( state == STATE_STOPPED ) // 
  {
    //cout << "State = STOPPED" << endl;
  }
}
