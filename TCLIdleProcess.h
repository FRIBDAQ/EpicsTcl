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

//  TCLIdleProcess.h
//
//   This file describes the CTCLIdleProcess class.
//   Idle processes are used in the TCL/TK environment
//   to describe operations which should be executed when
//   the TCL/TK Event dispatch loop has nothing to do.
// NOTE:
//  Since TCL/TK is unable to handle rescheduled Idle handlers,
//  this is actually implemented as a timer with schedule time of 0ms.
//
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
/////////////////////////////////////////////////////////////

#ifndef __TCLIDLEPROCESS_H
#define __TCLIDLEPROCESS_H

#ifndef __TCLTIMER_H // Required for derivation of current class.
#include "TCLTimer.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


class CTCLIntpreter;

// The class definition:


class CTCLIdleProcess : protected CTCLTimer
{
public:

  // Constructors:
  //
  CTCLIdleProcess(CTCLInterpreterObject* pInterp) :
    CTCLTimer(pInterp->getInterpreter()) {}

  CTCLIdleProcess(CTCLInterpreter* pInterp) :
    CTCLTimer(pInterp) {}

  virtual ~CTCLIdleProcess() {
				// Timer destructor clears.
  }

  // Copy Constructor [illegal]
private:
  CTCLIdleProcess(const CTCLIdleProcess& rRhs);
public:

  //  Assignment operator: [illegal]
private:
  CTCLIdleProcess& operator=(const CTCLIdleProcess& rRhs);
public:

  // Equality operator:

  int operator==(const CTCLIdleProcess& rRhs) {
    return CTCLInterpreterObject::operator==(rRhs);
  }

  // Operations and overrides:
public:
  void Set() {			// Delegate set/clear to timer,
    CTCLTimer::Set();		// Protected derivation makes
  }                             // Set(time) invisible. 
  void Clear() {
    CTCLTimer::Clear();
  }
  virtual void operator()() = 0; // Derive and supply for idle callback.
};


#endif
