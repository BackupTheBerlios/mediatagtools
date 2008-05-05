//
// C++ Interface: tools
//
// Description: 
//
//
// Author: Theofilos Intzoglou <int.teo@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

/*!
 * \relates TagLib::String
 *
 * Converts a TagLib::String to a QString without a requirement to link to Qt.
 */
#define Q4StringToTString(s) TagLib::String(s.toUtf8().data(), TagLib::String::UTF8)
