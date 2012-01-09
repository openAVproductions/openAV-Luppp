
#include "time.hpp"

#include "top.hpp"
#include "engineevent.hpp"

using namespace std;

Time::Time(Top* t)
{
  top = t;
}

// this function gets called by JACK before processing any audio for
// nframes, allowing it to modify Engine's state before it's run.
void Time::process(int frameNumber)
{
  float bpm = top->bpm;
  
  int framesPerBeat = (int) top->samplerate / (bpm / 60.0);
  
  int newBeat = frameNumber / framesPerBeat;
  
  // here we handle *all* events that should occur on *a* beat
  if ( newBeat != beat )
  {
    //cout << "Time:P() bpm = " << bpm << "  FPB: " << framesPerBeat << "  beat: " << newBeat << " Q1: " << q1.size() << "  Q4: " << q4.size() << endl;
    beat = newBeat;
    
    
    // always process q1 on new beat
    if ( !q1.empty() )
    {
      if ( !q1.empty() ) {
        //cout << "Time:process() doing list 1!" << endl;
        doEngineEventList(q1);
        q1.clear();
      }
    }
    
    if ( newBeat % 4 == 0 )
    {
      //cout << "Time:process() doing 4th list!" << endl;
      if ( !q4.empty() ) {
        doEngineEventList(q4);
      }
    }
    
    /*
    if ( newBeat % 8 == 0 )
    {
      cout << "Time:process() doing 8th list!  q8.size() = " << q8.size() << endl;
      if ( !q8.empty() ) {
        doEngineEventList(q8);
      }
      cout << " done processing 8 list, clearing NOW! " << endl;
      q8.clear();
    }
    
    if ( newBeat % 16 == 0 )
    {
      cout << "Time:process() doing 16th list!  q16.size() = " << q16.size() << endl;
      if ( !q16.empty() ) {
        doEngineEventList(q16);
      }
      cout << " done processing 16 list, clearing NOW! " << endl;
    }
    */
    
  } // new beat
  
}

void Time::processEngineEvent(EngineEvent* e)
{
  if ( e->type == EE_LOOPER_SELECT_BUFFER )
  {
    cout << "Time::processEE() LOOPER_SELECT_BUFFER queue in 4... type = " << e->type << "  T: " << e->ia << "  clip = " << e->ib << endl;
    if ( false ) // quantize check here
    {
      top->state.clipSelectorQueueClip(e->ia, e->ib);
      q4.push_back(e);
    }
    else
      doEngineEvent(e);
  }
  if ( e->type == EE_TRACK_SET_PLUGIN_PARAMETER )
  {
    //cout << "Time::processEE() SET_PLUGIN_PARAM queue in 1" << endl;
    //q1.push_back(e);
    doEngineEvent(e);
  }
  if ( e->type == EE_TRACK_DEVICE_ACTIVE )
  {
    doEngineEvent(e);
    //q1.push_back(e);
  }
}

// this function will do *all* events in a queue, and then return
void Time::doEngineEventList(std::list<EngineEvent*>& list)
{
  int eventCounter = 0;
  std::list<EngineEvent*>::iterator iter;
  for ( iter = list.begin(); iter != list.end(); iter++ )
  {
    eventCounter++;
    doEngineEvent( *iter );
  }
  
  //cout << "Time::doEngineEventList() Done " << eventCounter << " events, clearing NOW!" << endl;
  list.clear();
  
  return;
}

void Time::doEngineEvent(EngineEvent* e)
{
  //cout << "doEngineEvent()  type = " << e->type << endl;
  switch ( e->type )
  {
    case EE_LOOPER_PROGRESS: cout << "Time::doEngineEvent() got PROGRESS!!! ERROR!" << endl;;
    case EE_LOOPER_SELECT_BUFFER:
    {
      //cout << "Doing SELECT BUFFER NOW! t=" << e->ia << " clip#: " << e->ib << endl;
      top->state.clipSelectorActivateClip(e->ia, e->ib);
      break;
    }
    case EE_TRACK_DEVICE_ACTIVE: top->state.setPluginActive(e->ia, e->ib); break;
    case EE_TRACK_SET_PLUGIN_PARAMETER: top->state.setPluginParameter( e->ia, e->ic, e->fa ); break;
    default: cout << "Time::doEE() Error: got type " << e->type << " which is not handled yet!" << endl;
  }
}














