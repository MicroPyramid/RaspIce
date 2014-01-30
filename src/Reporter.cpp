/*------------------------------------------------------------------------------

   Copyright (c) 2000-2007 Tyrell Corporation. All rights reserved.

   Tyrell Reporter

   File     : Reporter.cpp
   Version  : $Revision: 553 $
   Author   : $Author: rafael@riseup.net $
   Location : $HeadURL: https://darkice.googlecode.com/svn/darkice/tags/darkice-1_2/src/Reporter.cpp $
   
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

#include <iostream>

#include "Reporter.h"


/* ===================================================  local data structures */


/* ================================================  local constants & macros */

/*------------------------------------------------------------------------------
 *  File identity
 *----------------------------------------------------------------------------*/
static const char fileid[] = "$Id: Reporter.cpp 553 2013-07-15 05:50:56Z rafael@riseup.net $";


/*------------------------------------------------------------------------------
 *  Initial values for static members of the class
 *----------------------------------------------------------------------------*/
unsigned int    Reporter::verbosity = 1;
std::ostream  * Reporter::os        = &std::cout;


/* ===============================================  local function prototypes */


/* =============================================================  module code */

    
