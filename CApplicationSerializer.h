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

// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
/*!
Stub class to stand in for the framework's global
application mutex interface.  Since we are not 
running in that threaded environment,  we don't 
need a mutext, however CSocket requires the interface
so we provide a dummy.
*/
#ifndef __CAPPLICATIONSERIALIZER_H  //Required for current class
#define __CAPPLICATIONSERIALIZER_H

//
// Include files:
//


class CApplicationSerializer      
{

public:

  static void Lock ()   ; // 
  static void Unlock ()   ; // 

};

#endif
