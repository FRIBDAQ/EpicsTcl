/*
#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL) ir the Tcl BSD derived license  The terms of these licenses
#     are described at:
#
#     GPL:  http://www.gnu.org/licenses/gpl.txt
#     Tcl:  http://www.tcl.tk/softare/tcltk/license.html
#     Start with the second paragraph under the Tcl/Tk License terms
#     as ownership is solely by Board of Trustees at Michigan State University. 
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#
*/


//  CErrnoException.cpp
//    Encapsulates exceptions which are thrown due to bad
//    errno values.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file///////////////////////////////////////////

//
// Header Files:
//





#include "ErrnoException.h"                               
#include <errno.h>

static const char* Copyright = 
"CErrnoException.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CErrnoException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char*  ReasonText (  )
//  Operation Type:
//     Selector.
//
const char*
CErrnoException::ReasonText() const 
{
  return strerror(m_nErrno);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector.
//
Int_t 
CErrnoException::ReasonCode() const 
{
  return m_nErrno;
}
