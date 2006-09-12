/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef SHARED_HPP
#define SHARED_HPP
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

// Shared pointer
#define SHARED_PTR(X,Y,W) class X; typedef boost::shared_ptr<X> Y; typedef boost::weak_ptr<X> W

// String-indexed map and iterator
#define SHARED_MAP(X,Y,Z) typedef std::map <std::string, X> Y; typedef std::map <std::string, X>::iterator Z

// Shared pointer, string-indexed map and iterator
#define SHARED_PTR_MAP(X,Y,Z,T,W) SHARED_PTR(X,Y,W); SHARED_MAP(Y,Z,T)
#define SHARED_MAP_PTR(X,Y,Z,T,W) SHARED_PTR_MAP(X,Y,Z,T,W)
//#define SHARED_PTR_MAP(X,Y,Z,T) class X; typedef boost::shared_ptr<X> Y; typedef std::map <std::string, Y> Z; typedef std::map <std::string, Y>::iterator T
//#define SHARED_MAP_PTR(X,Y,Z,T) class X; typedef boost::shared_ptr<X> Y; typedef std::map <std::string, Y> Z; typedef std::map <std::string, Y>::iterator T

#endif
