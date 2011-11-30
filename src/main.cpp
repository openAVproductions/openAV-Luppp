
#include <iostream>
#include <signal.h>

#include "jackclient.hpp"


using namespace std;

JackClient* jackClient = 0;

void sighandler(int sig)
{
  jackClient->deactivate();
  exit(0);
}

int main()
{
  cout << "Luppp 2.0" << endl;
  
  signal(SIGINT, &sighandler);
  
  jackClient = new JackClient();
  
  for ( ; ; )
  {
    sleep(1);
  }
  
  return 0;
}
