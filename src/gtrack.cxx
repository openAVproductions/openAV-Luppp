
#include "gtrack.hxx"
#include "gui.hxx"

static void gtrack_vol_cb(Fl_Widget *w, void *data);
static void gtrack_side_cb(Fl_Widget *w, void *data);
static void gtrack_post_cb(Fl_Widget *w, void *data);
static void gtrack_reverb_cb(Fl_Widget *w, void *data);


GTrack::GTrack(int x, int y, int w, int h, const char* l ) :
  Fl_Group(x, y, w, h),
  title( strdup(l) ),
  bg( x, y , w, h, title ),
  
  radial( x+5, y+ 26, 100, 100, ""),
  
  clipSel(x + 5, y + 26 + 102, 100, 294,""),
  
  volBox(x+5, y+422, 100, 172, ""),
  volume(x+66, y +425, 36, 166, ""),
  
  side(x+11, y +427 +  0, 50, 25, "Side"),
  post(x+21, y +435 + 50, 30, 30, "Post"),
  rev (x+21, y +440 +100, 30, 30, "Verb")
{
  ID = privateID++;
  
  clipSel.setID( ID );
  
  side.callback( gtrack_side_cb, this );
  side.setColor( 0, 0.6, 1 );
  
  rev.callback( gtrack_reverb_cb, this );
  post.callback( gtrack_post_cb, this );
  
  post.value( 1.0 );
  
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
  /*
  GTrack* track = (GTrack*) data;
  
  bool b = ((Avtk::Dial*)w)->value();
  float v = float( b );
  printf("track %i post send %s, %f\n", track->ID, b ? "true" : "false", v );
  
  EventTrackSend e( track->ID, SEND_SIDE, v );
  writeToDspRingbuffer( &e );
  */
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
