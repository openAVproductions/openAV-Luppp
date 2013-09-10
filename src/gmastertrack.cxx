
#include "gmastertrack.hxx"

static void gmastertrack_button_callback(Fl_Widget *w, void *data) {
  if ( strcmp( w->label(), "Metro" ) == 0 )
  {
    Avtk::Button* b = (Avtk::Button*)w;
    b->value( !b->value() );
    EventMetronomeActive e = EventMetronomeActive( b->value() );
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label(), "BPM" ) == 0 )
  {
    Avtk::Dial* b = (Avtk::Dial*)w;
    float bpm = b->value() * 160 + 60; // 60 - 220
    EventTimeBPM e = EventTimeBPM( bpm );
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label(), "Tap" ) == 0 )
  {
    EventTimeTempoTap e;
    writeToDspRingbuffer( &e );
  }
  else
  {
    cout << __FILE__ << __LINE__ << " Error: unknown command string" << endl;
  }
}

GMasterTrack::GMasterTrack(int x, int y, int w, int h, const char* l ) :
  Fl_Group(x, y, w, h),
  title( strdup(l) ),
  bg( x, y , w, h, title ),
  
  // with "true" master flag: launches scenes instead of clips on tracks
  clipSel(x + 5, y + 26 + 102, 140, 294,"", true),
  
  source(x+5, y+26, 140, 100, ""),
  volBox(x+5, y+422, 140, 172, ""),
  
  tapTempo       ( x + w * 2/4.f - 18, y + 426 + 41 * 0, 44,38, "Tap"),
  metronomeButton( x + w * 2/4.f - 18, y + 426 + 41 * 1, 44, 38,"Metro"),
  
  tempoDial      ( x + w * 2/4.f - 18, y + 426 + 41 * 2, 45, 36,"BPM"),
  metroVol       ( x + w * 2/4.f - 18, y + 426 + 41 * 3, 45, 36,"MtroVol"),
  
  inputVolume(x + 9,y + 26 + 4, w - 18, 29,""),
  volume(x+106, y +425, 36, 166, "")
{
  ID = privateID++;
  
  bar = 0;
  
  inputVolume.setOrientationHorizontal();
  
  tapTempo.callback( gmastertrack_button_callback, &ID );
  metronomeButton.callback( gmastertrack_button_callback, 0 );
  
  tempoDial.align( FL_ALIGN_CENTER );
  metroVol.align( FL_ALIGN_CENTER );
  
  for(int i = 0; i < 4; i++)
  {
    beatLights[i]   = new Avtk::LightButton( x + 11, y + 426 + 41 * i, 38, 38, "" );
  }
  beatLights[0]->setColor( 1.0, 0.0 , 0.0 );
  beatLights[1]->setColor( 1.0, 0.48, 0.0 );
  beatLights[2]->setColor( 1.0, 1.0 , 0.0 );
  beatLights[3]->setColor( 0.0, 1.0 , 0.0 );
  
  volBox.maximum(1.0f);
  volBox.minimum(0.0f);
  volBox.color( FL_BLACK );
  
  source.maximum(1.0f);
  source.minimum(0.0f);
  source.color( FL_BLACK );
  
  volume.amplitude( 0.0, 0.0 );
  
  end(); // close the group
}

void GMasterTrack::setBpm( int b )
{
  bpm = b;
  tempoDial.value( ( bpm - 60 ) / 140.f );
}

int GMasterTrack::getBpm()
{
  return bpm;
}

void GMasterTrack::setTapTempo( bool b )
{
  tapTempo.setHighlight( b );
}

void GMasterTrack::setBarBeat(int b, int beat)
{
  if ( beat % 4 == 0 )
  {
    bar = bar % 4 + 1;
  }
  
  int num = (beat % 4) + 1;
  
  //cout << bar << "  " << num << endl;
  
  // turn all off
  for( int i = 0; i < 4; i++)
    beatLights[i]->value( 0 );
  
  // beat starts at 4
  for( int i = 0; i < num; i++)
    beatLights[3-i]->value( 1 );
}

Avtk::Volume* GMasterTrack::getInputVolume()
{
  return &inputVolume;
}
Avtk::Volume* GMasterTrack::getVolume()
{
  return &volume;
}
Avtk::ClipSelector* GMasterTrack::getClipSelector()
{
  return &clipSel;
}

GMasterTrack::~GMasterTrack()
{
  free(title);
}
