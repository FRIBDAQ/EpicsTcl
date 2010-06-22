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

// #include <config.h>
#include "CLookupVisitor.h"    
#include "CChannel.h"

#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
				

// Functions for class CLookupVisitor

/*!
   Construct a lookup visitor... just salt away the timeout.

*/
CLookupVisitor::CLookupVisitor()
{
}


/*! 
  Called for each channel in the collection.
  - If the channel is not connected a Connect is done on it.
  - The caller must at some pont invoke CChannel::doEvents
 to allow Epics to dispatch event handler.s

*/
void 
CLookupVisitor::operator()(CChannel* pChannel)  
{ 
  if(!pChannel->isConnected()) {
    pChannel->Connect();
  }

}
