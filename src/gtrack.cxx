
#include "gtrack.hxx"
#include "gui.hxx"

static void gtrack_vol_cb(Fl_Widget *w, void *data);
static void gtrack_key_cb(Fl_Widget *w, void *data);
static void gtrack_xsideDial_cb(Fl_Widget *w, void *data);
static void gtrack_sendDial_cb(Fl_Widget *w, void *data);
static void gtrack_send_cb(Fl_Widget *w, void *data);
static void gtrack_record_cb(Fl_Widget *w, void *data);


GTrack::GTrack(int x, int y, int w, int h, const char* l ) :
  Fl_Group(x, y, w, h),
  bg( x, y , w, h, l ),
  
  radial( x+5, y+ 26, 100, 100, ""),
  
  clipSel(x + 5, y + 26 + 102, 100, 294,""),
  
  volBox(x+5, y+422, 100, 172, ""),
  volume(x+66, y +425, 36, 166, ""),
  
  
  sendDial   (x+21, y +430 +   0, 30, 30, ""),
  sendActive (x+11, y +430 +  32, 50, 25, "Snd"),
  
  xsideDial    (x+21, y +430 + 69, 30, 30, ""),
  keyActive  (x+11, y +430 + 101, 50, 25, "Key"),
  
  recordActive  (x+11, y +430 + 132, 50, 25, "Rec")
{
  ID = privateID++;
  
  clipSel.setID( ID );
  
  sendDial.callback( gtrack_sendDial_cb, this );
  sendActive.setColor( 0, 1.0, 0.0 );
  sendActive.callback( gtrack_send_cb, this );
  
  xsideDial.callback( gtrack_xsideDial_cb, this );
  xsideDial.align( FL_ALIGN_BOTTOM );
  keyActive.callback( gtrack_key_cb, this );
  keyActive.setColor( 0, 0.6, 1 );
  
  
  recordActive.setColor( 1, 0.0, 0.0 );
  recordActive.callback( gtrack_record_cb, this );
  
  volume.callback( gtrack_vol_cb, this );
  
  //volBox.color( fl_rgb_color( 0,0,0 ) );
  
  end(); // close the group
}

float GTrack::getSend(){return sendDial.value(); }
float GTrack::getXSide(){return xsideDial.value(); }

bool GTrack::getSendActive  (){return sendActive.value(); }
bool GTrack::getKeyActive   (){return keyActive.value(); }
bool GTrack::getRecordActive(){return recordActive.value(); }


void GTrack::setSend(float s){ sendDial.value( s ); }
void GTrack::setXSide(float s){ xsideDial.value( s ); }

void GTrack::setSendActive(bool a){ sendActive.value( a ); }
void GTrack::setKeyActive(bool a){ keyActive.value( a ); }
void GTrack::setRecordActive(bool a){ recordActive.value( a ); }


void gtrack_sendDial_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_POSTFADER, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i reverb send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_key_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  if ( b < 0.5 )
  {
    EventTrackSendActive e( track->ID, SEND_KEY, true );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSendActive e( track->ID, SEND_KEY, false );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i post send %s\n", track->ID, b ? "off" : "on" );
}


void gtrack_xsideDial_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_XSIDE, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i side send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}

void gtrack_vol_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackVol e( track->ID, ((Avtk::Volume*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i vol %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_send_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  d->value( !b );
  if ( b < 0.5 )
  {
    EventTrackSendActive e( track->ID, SEND_POSTFADER, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSendActive e( track->ID, SEND_POSTFADER, 0.0f );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i reverb send %s\n", track->ID, b ? "true" : "false" );
}
void gtrack_record_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  if ( b < 0.5 )
  {
    EventTrackRecordArm e( track->ID, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackRecordArm e( track->ID, 0.0f );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i record Arm %s\n", track->ID, b ? "off" : "on" );
}

