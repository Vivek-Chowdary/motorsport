/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include "logEngine.hpp"


/******************************************************************************
*
* Classes
*
******************************************************************************/

class dataEngine
{
	logEngine log;	//a log object for logging independently from other engines and main
	public:
		void start (void);		//starts the data engine
		/*here will probably go some new functions such as:

			int loadCarModel(carList*,FILE*carXmlFile);
			int loadTrackModel(track*,FILE*trackXmlFile);

		or maybe...

			int loadStreamingTrack(track*,FILE*trackXmlFile,currentPosition*,distance*);

		etc...*/
		void stop (void);		//stops the data engine
};
