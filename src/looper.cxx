

#include "looper.hxx"

#include "jack.hxx"
extern Jack* jack;

void Looper::setState(State s)
{
  // ensure we're not setting eg PLAY_QUEUED, if we're already PLAYING
  if ( static_cast<int>(s) != static_cast<int>(state) + 1)
  {
    cout << "new state " << s << endl;
    state = s;
  }
}

