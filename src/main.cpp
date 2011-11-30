
#include <iostream>
#include <signal.h>

#include "top.hpp"


using namespace std;

Top* top = 0;

void sighandler(int sig)
{
  top->jackClient->deactivate();
  exit(0);
}

int main()
{
  cout << "Luppp 2.0" << endl;
  
  signal(SIGINT, &sighandler);
  
  top = new Top();
  
  for ( ; ; )
  {
    sleep(1);
  }
  
  return 0;
}
