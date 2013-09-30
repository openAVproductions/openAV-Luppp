
#include "midi.hxx"

#include "../jack.hxx"

extern Jack* jack;

MidiObserver::MidiObserver()
{
  
}

void MidiObserver::registerPorts( std::string portName )
{
  jack->registerMidiObserver( this, portName );
}

void MidiObserver::port(int index)
{
  portIndex = index;
}

int MidiObserver::port()
{
  return portIndex;
}
