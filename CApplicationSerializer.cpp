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



////////////////////////// FILE_NAME.cpp /////////////////////////////////////////////////////
// #include <config.h>
#include "CApplicationSerializer.h"    				

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

 

// Functions for class CApplicationSerializer

/*! 
  
Simulate the global mutex lock by doing nothing

*/
void 
CApplicationSerializer::Lock()  
{ 
}  

/*! 
Simulate the global mutex unlock
by doing nothing.

*/
void 
CApplicationSerializer::Unlock()  
{ 
}
