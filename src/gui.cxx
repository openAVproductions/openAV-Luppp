

#include "gui.hxx"

Gui::Gui()
{
  window = new Fl_Window(340,180);
  box = new Fl_Box(20,40,300,100,"Luppp 5");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);
  window->end();
}

int Gui::show()
{
  window->show();
  return Fl::run();
}
