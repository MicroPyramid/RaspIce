/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell DarkIce

   File     : IceCast2.cpp
   Version  : $Revision: 553 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL: https://darkice.googlecode.com/svn/darkice/tags/darkice-1_2/src/IceCast2.cpp $
   
   Copyright notice:

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License  
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

------------------------------------------------------------------------------*/

/* ============================================================ include files */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#else
#error need stdio.h
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#error need string.h
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error need math.h
#endif


#include "Exception.h"
#include "Source.h"
#include "Sink.h"
#include "Util.h"
#include "IceCast2.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: IceCast2.cpp 553 2013-07-15 05:50:56Z rafael@riseup.net $";


/*------------------------------------------------------------------------------
 *  Expected positive response from server begins like this.
 *----------------------------------------------------------------------------*/
static const char responseOK[] = "HTTP/1.0 200";
static const char responseWrongPasswd[] = "HTTP/1.0 401";
static const char responseForbidden[] = "HTTP/1.0 403";

/* ===============================================  local function prototypes */


/* =============================================================  module code */

/*------------------------------------------------------------------------------
 *  Initialize the object
 *----------------------------------------------------------------------------*/
void
IceCast2 :: init (  StreamFormat            format,
                    const char            * mountPoint,
                    const char            * description )
                                                        throw ( Exception )
{
    this->format         = format;
    this->mountPoint     = Util::strDup( mountPoint);
    this->description    = description    ? Util::strDup( description) : 0;
}


/*------------------------------------------------------------------------------
 *  De-initialize the object
 *----------------------------------------------------------------------------*/
void
IceCast2 :: strip ( void )                           throw ( Exception )
{
    delete[] mountPoint;
    if ( description ) {
        delete[] description;
    }
}


/*------------------------------------------------------------------------------
 *  Log in to the IceCast2 server
 *----------------------------------------------------------------------------*/
bool
IceCast2 :: sendLogin ( void )                           throw ( Exception )
{
    Sink          * sink   = getSink();
    Source        * source = getSocket();
    const char    * str;
    const int       buflen = 1024;  // some small buffer size
    char            resp[buflen];   // a little buffer
    unsigned int    len;
    unsigned int    lenExpected;

    if ( !source->isOpen() ) {
        return false;
    }
    if ( !sink->isOpen() ) {
        return false;
    }

    // send the request, a string like:
    // "SOURCE <mountpoint> ICE/1.0"
    str = "SOURCE /";
    sink->write( str, strlen( str));
    str = getMountPoint();
    sink->write( str, strlen( str));
    str = " HTTP/1.0";
    sink->write( str, strlen( str));

    // send the content type, Ogg Vorbis
    str = "\nContent-type: ";
    sink->write( str, strlen( str));
    switch ( format ) {
        case mp3:
        case mp2:
            str = "audio/mpeg";
            break;

        case oggVorbis:
            str = "application/ogg";
            break;

        case oggOpus:
            str = "application/ogg";
            break;

        case aac:
            str = "audio/aac";
            break;

        case aacp:
            str = "audio/aacp";
            break;

        default:
            throw Exception( __FILE__, __LINE__,
                             "unsupported stream format", format);
            break;
    }
    sink->write( str, strlen( str));

    // send the authentication info
    str = "\nAuthorization: Basic ";
    sink->write( str, strlen(str));
    {
        // send source:<password> encoded as base64
        const char  * source = "source:";
        const char  * pwd    = getPassword();
        char        * tmp    = new char[Util::strLen(source) +
                                        Util::strLen(pwd) + 1];
        Util::strCpy( tmp, source);
        Util::strCat( tmp, pwd);
        char  * base64 = Util::base64Encode( tmp);
        delete[] tmp;
        sink->write( base64, strlen(base64));
        delete[] base64;
    }

    // send user agent info
    str = "\nUser-Agent: DarkIce/" VERSION " (http://code.google.com/p/darkice/)";
    sink->write( str, strlen( str));

    // send the ice- headers
    str = "\nice-bitrate: ";
    sink->write( str, strlen( str));
    
    sprintf( resp, "%d", getBitRate());
    sink->write( resp, strlen( resp));

    str = "\nice-public: ";
    sink->write( str, strlen( str));
    str = getIsPublic() ? "1" : "0";
    sink->write( str, strlen( str));

    if ( getName() ) {
        str = "\nice-name: ";
        sink->write( str, strlen( str));
        str = getName();
        sink->write( str, strlen( str));
    }

    if ( getDescription() ) {
        str = "\nice-description: ";
        sink->write( str, strlen( str));
        str = getDescription();
        sink->write( str, strlen( str));
    }

    if ( getUrl() ) {
        str = "\nice-url: ";
        sink->write( str, strlen( str));
        str = getUrl();
        sink->write( str, strlen( str));
    }

    if ( getGenre() ) {
        str = "\nice-genre: ";
        sink->write( str, strlen( str));
        str = getGenre();
        sink->write( str, strlen( str));
    }

    str = "\n\n";
    sink->write( str, strlen( str));
    sink->flush();

    // read the response, expected response begins with responseOK
    lenExpected = Util::strLen( responseOK);
    if ( (len = source->read( resp, buflen )) < lenExpected ) {
        return false; // short read, no need to continue
    }
    resp[lenExpected] = '\x00'; // end string, truncate to expected length

    reportEvent(5,resp);

    if ( Util::strEq( resp, responseWrongPasswd) ) {
	throw Exception( __FILE__, __LINE__,
                         "Icecast2 - wrong password");
    }

    if ( Util::strEq( resp, responseForbidden) ) {
	throw Exception( __FILE__, __LINE__,
                         "Icecast2 - forbidden. Is the mountpoint occupied, or maximum sources reached?");
    }

    if ( !Util::strEq( resp, responseOK) ) {
        // some unexpected response from server
        throw Exception( __FILE__, __LINE__,
                         "Icecast2 - Unexpected response from server");
    }
    
    // suck anything that the other side has to say
    while ( source->canRead( 0, 0) && 
           (len = source->read( resp, buflen )));
    
    // all is well, we are connected
    return true;
}


