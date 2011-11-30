
#include "top.hpp"

#include "jackclient.hpp"

using namespace std;

Top::Top()
{
  //cout << "Top()" << endl;
  
  jackClient = new JackClient(this);
}

