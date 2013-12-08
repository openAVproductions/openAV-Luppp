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


#ifndef AVTK_CLIP_SELECTOR_H
#define AVTK_CLIP_SELECTOR_H

#include <string>
#include <sstream>
#include <FL/Fl_Button.H>

#include "../config.hxx"

#include "../looper.hxx"
#include "../gridlogic.hxx"
#include "../audiobuffer.hxx"
#include "../eventhandler.hxx"

using namespace std;

namespace Avtk
{

class ClipState
{
  public:
    ClipState() :
      state(GridLogic::STATE_EMPTY),
      name("")
    {}
    
    void setName(std::string n)
    {
      name = n;
    }
    
    void setState(GridLogic::State s)
    {
      state = s;
    }
    
    std::string getName()
    {
      return name;
    }
    
    GridLogic::State getState()
    {
      return state;
    }
  
  private:
    GridLogic::State state;
    std::string name;
};

class ClipSelector : public Fl_Button
{
  public:
    ClipSelector( int _x, int _y, int _w, int _h,
                  const char *_label, bool master = false);
    
    int ID;
    
    // FIXME: NSCENES?
    static const int numClips = 10;
    ClipState clips[numClips];
    
    /// indicates if a clip is the "special" clip
    void setSpecial(int scene);
    int special;
    
    bool _master;
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void setID( int id );
    
    /** converts the Looper::State into the UI represnted ClipSelector state.
     * It puts some of the data into clips[], and stores unique state into the class.
    **/
    void setState( int clipNum, GridLogic::State s );
    
    void clipName(int clip, std::string name);
    std::string clipName(int clip);
    
    void draw();
    int handle(int event);
    
    void resize(int X, int Y, int W, int H);
};

} // Avtk

#endif // AVTK_CLIP_SELECTOR_H

