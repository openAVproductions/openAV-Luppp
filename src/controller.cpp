
#include "controller.hpp"

#include "top.hpp"

using namespace std;

Controller::Controller(Top* t)
{
  cout << "Controller()" << endl;
  
  top = t;
  
  track = 0;
  device = 0;
}
