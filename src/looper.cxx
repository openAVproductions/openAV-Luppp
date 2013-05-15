

#include "looper.hxx"

#include "jack.hxx"
extern Jack* jack;

void Looper::setState(State s)
{
  // before update, check if we recording, if so, print info
  /*
  if ( state == STATE_RECORDING )
  {
    int newBpm = 120;// (lastWrittenSampleIndex / (44100/2) ) * 60;
    
    cout << "Looper " << track << " ending record: endPoint @ " << lastWrittenSampleIndex
         << ". Bpm " << newBpm << " perhaps?" << endl;
    
    jack->getTimeManager()->setBpm( newBpm );
  }
  */
  
  // quantize?!
  state = s;
}
