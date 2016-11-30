/*
 * Author: Harry van Haaren 2016
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

#include "ctlrscript.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../event.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"

extern Jack* jack;

CtlrScript::CtlrScript(std::string filename) :
  Controller()
{
	printf("%s, attempting to compile %s\n", __func__, filename.c_str());

#warning WIP HERE, add TCC libs, and attempt to load/compile filename
}

void CtlrScript::bpm(int bpm)
{
	printf("%s : %d\n", __func__, bpm);
}
