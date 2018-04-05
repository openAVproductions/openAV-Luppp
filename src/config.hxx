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

#ifndef LUPPP_CONFIG_H
#define LUPPP_CONFIG_H


///     PROGAM WIDE DEFINES
#define NAME "Luppp"

#pragma GCC diagnostic ignored "-Wunused-parameter"


///     DEBUG
// Track operations
//#define DEBUG_TRACKS            1

// Clip selection / queueing
//#define DEBUG_CLIP              1

// Buffer loading / resizing
//#define DEBUG_BUFFER            1

// Logic : Warning NON RT!
//#define DEBUG_LOGIC             1

// State save / load
//#define DEBUG_STATE             1

// Jack print controller out MIDI, loading etc
//#define DEBUG_MIDI              1


///     GENERAL
#define NTRACKS 8
#define NSCENES 10
#define MAX_BUFFER_SIZE 1024

///     TEMPO
#define MIN_TEMPO 60
#define MAX_TEMPO 220

#define CONTROLLERS_PREALLOC 20

// nsamples remaining during recording before Looper requests larger buffer
#define LOOPER_SAMPLES_BEFORE_REQUEST 44100
#define LOOPER_SAMPLES_UPDATE_SIZE    44100

#define LUPPP_RETURN_OK       0
#define LUPPP_RETURN_WARNING  1
#define LUPPP_RETURN_ERROR    2


///     debug.hxx for printing convienience
#include "debug.hxx"


#endif // LUPPP_CONFIG_H

