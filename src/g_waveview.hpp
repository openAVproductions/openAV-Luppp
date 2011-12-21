#ifndef LUPPP_WAVEVIEW
#define LUPPP_WAVEVIEW

#include <gtkmm/drawingarea.h>

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
  
  // Button & Expose event.
  bool on_button_press_event(GdkEventButton* event);
  bool on_expose_event(GdkEventExpose* event);
};

#endif // GTKMM_EXAMPLE_WaveView_H
