/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "controllerupdater.hxx"

#include "eventhandler.hxx"

ControllerUpdater::ControllerUpdater()
{
  // CAREFUL size of controllers: otherwise malloc() called
  c.reserve( CONTROLLERS_PREALLOC );
}

void ControllerUpdater::registerController( Controller* controller )
{
  if (!controller)
  {
    LUPPP_ERROR("Register Controller passed NULL controller!");
    return;
  }
  
  //LUPPP_NOTE("Registering controller %s", controller->getName().c_str() );
  
  // store the controller instance
  c.push_back( controller );
  
  // and tell it to register itself (MidiObserver / AudioObserver etc)
  controller->registerComponents();
}

void ControllerUpdater::removeController( int id )
{
  for( unsigned int i = 0; i < c.size(); i++)
  {
    if ( c.at(i)->getID() == id )
    {
      // keep pointer: otherwise we mem-leak
      Controller* removed = c.at( i );
      
      cout << "Removing item " << i << " with name " << removed->getName() << endl;
      
      // remove the instance at 
      c.erase( c.begin() + i );
      
      // send it for de-allocation
      EventControllerInstance e( removed );
      writeToGuiRingbuffer( &e );
    }
  }
  
}

Controller* ControllerUpdater::getController(int id)
{
  // search controllers for ID, if found return a pointer to it
  for( unsigned int i = 0; i < c.size(); i++)
  {
    if ( c.at(i)->getID() == id )
    {
      LUPPP_NOTE("getController target ID: %i, found ID: %i, returning %s", id, i, c.at(i)->getName().c_str() );
      return c.at(i);
    }
  }
  return 0;
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

void ControllerUpdater::masterReturnVolume(float v)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->masterReturnVolume( v );
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

void ControllerUpdater::specialScene(int t, int scene)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->specialScene(t, scene);
}

void ControllerUpdater::masterInputToActive(int to, bool v)
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->masterInputToActive( to, v);
}
void ControllerUpdater::masterInputTo( int to, float v )
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->masterInputTo( to, v);
}
void ControllerUpdater::masterInputVol( float v )
{
  for(unsigned int i = 0; i < c.size(); i++)
    c.at(i)->masterInputVol( v );
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
