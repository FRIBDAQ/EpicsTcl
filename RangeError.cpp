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


//  CRangeError.cpp
// Encapsulates range errors of various sorts.
// These include array limit errors as well as
//  numerical range errrors.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//



#include "RangeError.h"                               
#include <stdio.h>
#include <assert.h>

static const char* Copyright=
"CRangeError.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CRangeError

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     Selector.
//
const char* 
CRangeError::ReasonText() const 
{
// Returns a string which looks something like:
// "Range error: n is outside the range [low..high]"
//
// Exceptions:  

  return m_ReasonText.c_str();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CRangeError::ReasonCode() const 
{
// Returns:
//      CRangeError::knTooLow:  
//           m_nRequested < m_nLow
//      CRangeError::knTooHigh:
//           m_nRequested > m_nHigh
//

  if(m_nRequested > m_nHigh) {
    return knTooHigh;
  }
  else if (m_nRequested < m_nLow) {
    return knTooLow;
  }
  else {
    assert( 0 != 0);		// Exception was thrown in error!!
  }

}

///////////////////////////////////////////////////////////////////////
//
// Function:
//   void UpdateReason()
// Operation:
//   Protected utility:
//
void
CRangeError::UpdateReason()
{
  char szMessage[100];
  sprintf(szMessage, "Range error: %d is outside the range [ %d..%d]",
	  m_nRequested, m_nLow, m_nHigh);
  m_ReasonText = szMessage;

}

