
#include "../jack.hxx"

#include <iostream>

extern Jack* jack;

AkaiAPC::AkaiAPC()
{
  
}

void AkaiAPC::record(int t, bool b)
{
  unsigned char data[3];
  data[0] = 144 + t;
  data[1] = 48; // record LED
  data[2] = 127 * b;
  jack->writeApcOutput( &data[0] );
  cout << "record written to JACK" << endl;
}

void AkaiAPC::mute(int t, bool b)
{
  
}

void AkaiAPC::clipSelect(int t, bool b)
{
  
}

void AkaiAPC::volume(int t, float f)
{
  
}
