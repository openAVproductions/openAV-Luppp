
#include "gtrack.hxx"
#include "gui.hxx"

extern Gui* gui;

void gtrack_button_callback(Fl_Widget *w, void *data)
{
  int track = 0;
  if ( data )
    track = *(int*)data;
  //cout << "Button " << *(int*)data << " " << w->label() << " clicked" << endl;
  
  if ( strcmp( w->label() , "Rec" ) == 0 )
  {
    EventLooperState e = EventLooperState(track,Looper::STATE_RECORD_QUEUED);
    writeToDspRingbuffer( &e );
    //w->tooltip( "Rec Clicked" );
    gui->setTooltip("Rec clicked");
  }
  else if ( strcmp( w->label() , "Play" ) == 0 )
  {
    EventLooperState e = EventLooperState(track,Looper::STATE_PLAY_QUEUED);
    writeToDspRingbuffer( &e );
    //w->tooltip( "Play clicked" );
    gui->setTooltip("Play clicked");
  }
  else if ( strcmp( w->label() , "Stop" ) == 0 )
  {
    EventLooperState e = EventLooperState(track,Looper::STATE_STOP_QUEUED);
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label() , "+" ) == 0 )
  {
    EventLooperLoopLength e = EventLooperLoopLength(track, 2);
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label() , "-" ) == 0 )
  {
    EventLooperLoopLength e = EventLooperLoopLength(track, 0.5);
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label() , "Load" ) == 0 )
  {
    AudioBuffer* ab = Worker::loadSample( choose_file() );
    EventLooperLoad e = EventLooperLoad( track, 0 , ab );
    cout << "writing event ab ptr = " << ab  << endl;
    writeToDspRingbuffer( &e );
    cout << "writing event done" << endl;
  }
  else if ( strcmp( w->label() , "Vol" ) == 0 )
  {
    
  }
  else
  {
    cout << __FILE__ << __LINE__ << " Error: unknown command string" << endl;
  }
}
