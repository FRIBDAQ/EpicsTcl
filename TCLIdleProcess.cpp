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
//
//  TCLIdleProcess.cpp:
//     This file provides implementions for the non trivial member functions
//     of the CTCLIdleProcess class.  This class provides an object oriented
//     encapsulation of the Tk Idle process object.  Idle processes are
//     called once and only once when the Tk Event loop has no events to
//     process.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////////////////////


#include <iostream>

using namespace std;

#include "TCLIdleProcess.h"


