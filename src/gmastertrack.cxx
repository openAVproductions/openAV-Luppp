
#include "gmastertrack.hxx"

static void gmastertrack_tempoDial_callback(Fl_Widget *w, void *data)
{
  Avtk::Dial* b = (Avtk::Dial*)w;
  float bpm = b->value() * 160 + 60; // 60 - 220
  EventTimeBPM e = EventTimeBPM( bpm );
  writeToDspRingbuffer( &e );
}

static void gmastertrack_volume_callback(Fl_Widget *w, void *data)
{
  Avtk::Volume* b = (Avtk::Volume*)w;
  float v = b->value();
  EventMasterVol e( v );
  writeToDspRingbuffer( &e );
}

static void gmastertrack_recordVol_callback(Fl_Widget *w, void *data)
{
  //Avtk::Dial* b = (Avtk::Dial*)w;
  //b->value( !b->value() );
  //EventMetronomeActive e = EventMetronomeActive( b->value() );
  //writeToDspRingbuffer( &e );
}

static void gmastertrack_mixVol_callback(Fl_Widget *w, void *data)
{
  //Avtk::Dial* b = (Avtk::Dial*)w;
  //b->value( !b->value() );
  //EventMetronomeActive e = EventMetronomeActive( b->value() );
  //writeToDspRingbuffer( &e );
}

static void gmastertrack_recordButton_callback(Fl_Widget *w, void *data)
{
  Avtk::LightButton* b = (Avtk::LightButton*)w;
  b->value( !b->value() );
  //EventMetronomeActive e = EventMetronomeActive( b->value() );
  //writeToDspRingbuffer( &e );
}

static void gmastertrack_mixButton_callback(Fl_Widget *w, void *data)
{
  Avtk::LightButton* b = (Avtk::LightButton*)w;
  b->value( !b->value() );
  //EventMetronomeActive e = EventMetronomeActive( b->value() );
  //writeToDspRingbuffer( &e );
}

static void gmastertrack_button_callback(Fl_Widget *w, void *data)
{
  if ( strcmp( w->label(), "Metro" ) == 0 )
  {
    Avtk::LightButton* b = (Avtk::LightButton*)w;
    b->value( !b->value() );
    EventMetronomeActive e = EventMetronomeActive( b->value() );
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
  returnVol      ( x + w * 2/4.f - 18, y + 426 + 41 * 3, 45, 36,"Return"),
  
  inputVolume(x + 9,y + 26 + 4, w - 18, 29,""),
  inputToRecord(x + 9,y + 26 + 36, w * 0.6, 29,"to Record"),
  inputToMix   (x + 9,y + 26 + 68, w * 0.6, 29,"to Master"),
  
  inputToRecordVol(x + w*0.6,y + 26 + 36, w / 2, 29,""),
  inputToMixVol   (x + w*0.6,y + 26 + 68, w / 2, 29,""),
  
  volume(x+106, y +425, 36, 166, "")
{
  ID = privateID++;
  
  bar = 0;
  
  inputVolume.value(0.5);
  inputVolume.setOrientationHorizontal();
  
  tapTempo.callback( gmastertrack_button_callback, &ID );
  metronomeButton.callback( gmastertrack_button_callback, 0 );
  inputToRecord.callback( gmastertrack_recordButton_callback, 0 );
  inputToMix.callback   ( gmastertrack_mixButton_callback, 0 );
  
  tempoDial.callback( gmastertrack_tempoDial_callback, 0 );
  inputToRecordVol.callback( gmastertrack_recordVol_callback, 0 );
  inputToMixVol.callback   ( gmastertrack_mixVol_callback, 0 );
  
  inputToMix.value( 1 );
  inputToMixVol.value( 1 );
  inputToRecord.value( 1 );
  inputToRecordVol.value( 1 );
  
  tempoDial.align( FL_ALIGN_CENTER );
  returnVol.align( FL_ALIGN_CENTER );
  
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
  volume.callback( gmastertrack_volume_callback, 0 );
  
  end(); // close the group
}

void GMasterTrack::setBpm( int b )
{
  bpm = b;
  tempoDial.value( ( bpm - 60 ) / 160.f );
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
