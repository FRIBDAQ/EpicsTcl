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

static const char* Copyright= "(C) Copyright Michigan State University 1936, All rights reserved";

// Class: CStreamIOError
// Encapsulates error conditions on an ios
// derived object as an exception.
// Note that objects of this type carry with them
// a reference to the stream on which the condition
// was detected.  This reference may become invalid
//  if the exception propagates up the call stack
//  far enough to cause the stream to be destroyed.
//  I would have ideally liked to carry a copy of the stream,
//  however far too few ios derived classes are derived
//  from ***_withassign to make this practical.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////



#include "StreamIOError.h"    				
#include <string.h>

static const char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved StreamIOError.cpp \n";

static  char* pErrorMessages[] =
{
  "End of file encountered (prematurely) ",
  "Bad bit set in ios:: ",
  "Fail bit set in ios:: "
};

char** CStreamIOError::m_svErrorMessages = pErrorMessages; //Static data member initialization

// Functions for class CStreamIOError
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    const char* getErrorMessage() const
// Operation Type:
//    Selector
//
const char*
CStreamIOError::getErrorMessage() const
{
  // Returns the error message string (only, no doing context)
  //
  if( (m_eReason < EndFile) || (m_eReason > FailSet) ) {
    return "Invalid error enumerator!!!";
  }
  return m_svErrorMessages[m_eReason];
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonText()
//  Operation Type: 
//     selector
const char* CStreamIOError::ReasonText()   const
{
  // Returns error text describing this error.
  // in this case this is a string consisting of the
  // error reason and the action at the time the
  // exception was thrown.
  
  return getErrorMessage();

}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonCode()
//  Operation Type: 
//     selector
Int_t CStreamIOError::ReasonCode()  const
{
  // Casts the error reason to an int and returns it.
  
  return (Int_t)m_eReason;
}
