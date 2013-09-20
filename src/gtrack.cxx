
#include "gtrack.hxx"
#include "gui.hxx"

static void gtrack_vol_cb(Fl_Widget *w, void *data);
static void gtrack_side_cb(Fl_Widget *w, void *data);
static void gtrack_post_cb(Fl_Widget *w, void *data);
static void gtrack_reverb_cb(Fl_Widget *w, void *data);
static void gtrack_active_cb(Fl_Widget *w, void *data);
static void gtrack_record_cb(Fl_Widget *w, void *data);


GTrack::GTrack(int x, int y, int w, int h, const char* l ) :
  Fl_Group(x, y, w, h),
  bg( x, y , w, h, l ),
  
  radial( x+5, y+ 26, 100, 100, ""),
  
  clipSel(x + 5, y + 26 + 102, 100, 294,""),
  
  volBox(x+5, y+422, 100, 172, ""),
  volume(x+66, y +425, 36, 166, ""),
  
  rev      (x+21, y +430 +  0, 30, 30, ""),
  active   (x+11, y +430 + 32, 50, 25, "Snd"), // active
  
  post     (x+21, y +430 + 69, 30, 30, ""),
  side     (x+11, y +430 + 101, 50, 25, "Key"),
  
  recEnable(x+11, y +430 + 132, 50, 25, "Rec") // record
{
  ID = privateID++;
  
  clipSel.setID( ID );
  
  side.callback( gtrack_side_cb, this );
  side.setColor( 0, 0.6, 1 );
  
  active.setColor( 0, 1.0, 0.0 );
  active.callback( gtrack_active_cb, this );
  recEnable.setColor( 1, 0.0, 0.0 );
  recEnable.callback( gtrack_record_cb, this );
  
  rev.callback( gtrack_reverb_cb, this );
  post.callback( gtrack_post_cb, this );
  post.align( FL_ALIGN_BOTTOM );
  
  volume.callback( gtrack_vol_cb, this );
  
  volBox.maximum(1.0f);
  volBox.minimum(0.0f);
  volBox.color( FL_BLACK );
  volBox.selection_color( FL_BLUE );
  
  end(); // close the group
}

void gtrack_reverb_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_REV, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  printf("track %i reverb send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_side_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  if ( b < 0.5 )
  {
    EventTrackSend e( track->ID, SEND_SIDE, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSend e( track->ID, SEND_SIDE, 0.0f );
    writeToDspRingbuffer( &e );
  }
  printf("track %i post send %s\n", track->ID, b ? "true" : "false" );
}


void gtrack_post_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_POST, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  printf("track %i side send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}
void gtrack_vol_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackVol e( track->ID, ((Avtk::Volume*)w)->value() );
  writeToDspRingbuffer( &e );
  printf("track %i vol %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}
void gtrack_active_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  d->value( !b );
  /*
  if ( b < 0.5 )
  {
    EventTrackSend e( track->ID, SEND_SIDE, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSend e( track->ID, SEND_SIDE, 0.0f );
    writeToDspRingbuffer( &e );
  }
  printf("track %i post send %s\n", track->ID, b ? "true" : "false" );
  */
}
void gtrack_record_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  d->value( !b );
  //EventTrackVol e( track->ID, ((Avtk::Volume*)w)->value() );
  //writeToDspRingbuffer( &e );
  //printf("track %i vol %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}
