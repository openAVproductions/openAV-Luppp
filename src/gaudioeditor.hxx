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


#ifndef LUPPP_AUDIO_EDITOR_H
#define LUPPP_AUDIO_EDITOR_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class AudioBuffer;
namespace Avtk
{
class Waveform;
class Button;
}

class AudioEditor
{
public:
	AudioEditor();
	~AudioEditor();

	/// shows the window, and loads the audio buffer into the display
	void show( AudioBuffer* ab, bool modal = false );

	void hide();

	/// returns true if the editor window is shown
	bool shown();

	void setBeatsAndQuit( int beats );

	//AudioBuffer* getAudioBuffer();

private:
	// GUI elements
	Fl_Double_Window* window;
	Avtk::Waveform*   waveform;
	Avtk::Button*     cancel;
	Avtk::Button*     beatButtons[7];

	// Contents
	AudioBuffer* ab;
};

#endif // LUPPP_AUDIO_EDITOR_H

