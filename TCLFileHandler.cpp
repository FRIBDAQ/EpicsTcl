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

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
//  CTCLFileHandler.cpp
// Abstract base class for TK file handerl objects.
// These are fd containing objects which are inserted into
//  the TK event stream.  They allow response to events
// such as readability, writability and exceptions on a file
// descriptor based object.
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




#include "TCLFileHandler.h"                               
#include <tcl.h>
#include <tk.h>


using namespace std;


// Functions for class CTCLFileHandler


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void CallbackRelay ( ClientData pObject, int mask )
//  Operation Type:
//     Contextualizer
//
void 
CTCLFileHandler::CallbackRelay(ClientData pObject, int mask) 
{
// Establishes object context from the client data and 
// calls the appropriate object's operator() method.
// 
// Formal Parameters:
//      ClientData  pObject:
//           Pointer to the object for which the callback
//           is intended.
//      int mask:
//           Describes the set of events which might have occured.
//           See operator() for description.
//
// Exceptions:  

  CTCLFileHandler* pThat = (CTCLFileHandler*)pObject;
  (*pThat)(mask);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Set ( int mask )
//  Operation Type:
//     Enabler
//
void 
CTCLFileHandler::Set(int mask) 
{
// Enables the call back for the file descriptor with
// the associated set of events.
//
// Formal Parameters:
//        int mask:
//           Mask of events for which to enable the 
//           callback.  Note that 0 will temporarily disable
//           the callback without removing it from Tcl/TK's 
//           callback database.  This is supposed to be more efficient
//           than a call to Clear().

#ifdef WIN32
  assert(0);			// CYGWIN's does not implement this.
#else
  Tk_CreateFileHandler(m_nFid, mask, CallbackRelay, (ClientData)this);
#endif
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Clear (  )
//  Operation Type:
//     Disabler
//
void 
CTCLFileHandler::Clear() 
{
// Disables the callback for this object.  
// Note: that this is called automatically by the
//           destructor.
// Note:  That the only one file handler can be
//             associated with a file descriptor.
// 
#ifdef WIN32
  assert(0);			// CYGWIn's tcl does not implement this.
#else
  Tk_DeleteFileHandler(m_nFid);
#endif

}
