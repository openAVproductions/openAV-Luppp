

#include "gui.hxx"

Gui::Gui()
{
  window = new Fl_Window(640,280);
  window->color(FL_BLACK);
  box = new Fl_Box(5, 5, 200, 60, "Luppp 5");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);
  
  track  = new GTrack(5,80,200,200);
  track2 = new GTrack(120,80,200,200);
  
  window->end();
}

int Gui::show()
{
  window->show();
  return Fl::run();
}
