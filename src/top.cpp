
#include "top.hpp"

#include "jackclient.hpp"
#include "mixer.hpp"

using namespace std;

Top::Top()
{
  //cout << "Top()" << endl;
  
  jackClient = new JackClient(this);
  cout << "\t\t\tDone" << endl;
  
  addTrack();
  addTrack();
}

void Top::addTrack()
{
  ((Mixer*)mixer)->addTrack();
  state.addTrack();
}
