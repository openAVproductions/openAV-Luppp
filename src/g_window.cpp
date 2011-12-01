
#include "g_window.hpp"

Window::Window(Gtk::Main *k, Top* t)
{
  // store the "kit" instance from main
  kit = k;
  
  // keep the top pointer for communication
  top = t;
  
  
  // load Glade file
  Glib::RefPtr<Gtk::Builder> refBuilder;
  
  try {
    refBuilder = Gtk::Builder::create_from_file("ui.glade");
  }
  catch (Gtk::BuilderError e) {
    std::cout << "Gtk::BuilderError loading mainwindow glade file: " << e.what() << std::endl;
    exit(1);
  }
  catch (Glib::FileError e) {
    std::cout << "Gtk::FileError loading mainwindow glade file: " << e.what() << std::endl;
    exit(1);
  }
  
  // get the widgets
  refBuilder->get_widget("window", window);
  window->set_title("Luppp 2.0");
  
  
  
  kit->run(*window);
}

void Window::addTrack()
{
  //trackoutputList.push_back( TrackOutput(top) );
  
}

