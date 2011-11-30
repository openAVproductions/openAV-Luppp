
#include "top.hpp"

using namespace std;

Top::Top()
{
  //cout << "Top()" << endl;
  
  cout << "Creating JackClient... " << flush;
  jackClient = new JackClient();
  cout << "\t\t\tDone" << endl;
}

