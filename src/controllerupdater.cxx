
#include "controllerupdater.hxx"

ControllerUpdater::ControllerUpdater()
{
}

void ControllerUpdater::registerController( Controller* controller )
{
  // store the controller instance
  c.push_back( controller );
  
  // and tell it to register itself (MidiObserver / AudioObserver etc)
  controller->registerComponents();
}

void ControllerUpdater::reset()
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->reset();
}

void ControllerUpdater::mute(int t, bool b)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->mute(t,b);
}

void ControllerUpdater::masterVolume(float v)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->masterVolume(v);
}

void ControllerUpdater::setTrackSceneProgress(int t, int s, float p)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->progress(t,s,p);
}

void ControllerUpdater::setTrackSendActive(int t, int send, bool v)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->trackSendActive(t, send, v);
}
void ControllerUpdater::setTrackSend(int t, int send, float v)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->trackSend(t, send, v);
}

void ControllerUpdater::launchScene( int scene )
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->launchScene(scene);
}

void ControllerUpdater::setSceneState(int t, int clip, GridLogic::State s)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->setSceneState(t,clip,s);
}

void ControllerUpdater::recordArm(int t, bool r)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->recordArm(t,r);
}

void ControllerUpdater::volume(int t, float v)
{
  for(unsigned int i = 0; i < c.size(); i++) c.at(i)->volume(t,v);
}

void ControllerUpdater::tapTempo(bool b)
{
  for(unsigned int i = 0; i < c.size(); i++) c.at(i)->tapTempo(b);
}

void ControllerUpdater::metronomeEnable(bool b)
{
  for(unsigned int i = 0; i < c.size(); i++) c.at(i)->metronomeEnable(b);
}
