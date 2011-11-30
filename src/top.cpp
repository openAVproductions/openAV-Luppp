
#include "top.hpp"

#include "jackclient.hpp"

using namespace std;

Top::Top()
{
  //cout << "Top()" << endl;
  
  cout << "Creating JackClient... " << flush;
  jackClient = new JackClient(this);
  cout << "\t\t\tDone" << endl;
}

