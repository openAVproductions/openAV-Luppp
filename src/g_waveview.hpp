#ifndef LUPPP_WAVEVIEW
#define LUPPP_WAVEVIEW

#include <gtkmm/drawingarea.h>
#include <gtkmm.h>

class GWaveView : public Gtk::DrawingArea
{
public:
  GWaveView();
  ~GWaveView();
  
  void redraw();
  
  void setSample(const std::vector<float>& inSample);

protected:
  // the sample "loaded" is the real one. NOT a copy (ie: ByRef)
  std::vector<float> sample;
  
  int waveviewSize;
  
  void setSize(int);
  
  // popup menu stuff:
  Gtk::Menu      pMenu;
  Gtk::MenuItem  setSize1Sec;
  Gtk::MenuItem  setSizeHalfSec;
  Gtk::MenuItem  setSizeQuaterSec;
  Gtk::MenuItem  setSizeEigthSec;
  
  // Button & Expose event.
  bool on_button_press_event(GdkEventButton* event);
  bool on_expose_event(GdkEventExpose* event);
};

#endif // GTKMM_EXAMPLE_WaveView_H
