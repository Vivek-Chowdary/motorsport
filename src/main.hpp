/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef MAIN_HPP
#   define MAIN_HPP
#   include "data/xercesc_fwd.hpp"

//forward declarations
class LogEngine;

void stopSdl (void);
void startSdl (LogEngine * log);
LogEngine * processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * node);

#endif
