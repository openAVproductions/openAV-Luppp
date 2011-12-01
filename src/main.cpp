
#include <iostream>
#include <signal.h>

#include "top.hpp"
#include "jackclient.hpp"


using namespace std;

static Top* top = 0;
static JackClient* jackClient = 0;

void sighandler(int)
{
  jackClient->deactivate();
  exit(0);
}

int main()
{
  cout << "Luppp 2.0" << endl;
  
  signal(SIGINT, &sighandler);
  
  top = new Top();
  
  jackClient = new JackClient(top);
  
  for ( ; ; )
  {
    sleep(1);
  }
  
  return 0;
}
