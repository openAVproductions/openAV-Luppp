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

#ifndef LUPPP_EVENT_HANDLER_H
#define LUPPP_EVENT_HANDLER_H

#include "event.hxx"
#include <jack/ringbuffer.h>

extern char* processDspMem;
extern char* processGuiMem;

extern jack_ringbuffer_t* rbToDsp;
extern jack_ringbuffer_t* rbToGui;

// to process events in the ringbuffer
extern void handleDspEvents();
extern void handleGuiEvents();

// to write events to the ringbuffer
extern void writeToDspRingbuffer(EventBase* e);
extern void writeToGuiRingbuffer(EventBase* e);

#endif // LUPPP_EVENT_HANDLER_H

