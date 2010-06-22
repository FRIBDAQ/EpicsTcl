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
//  CTCLInterpreterObject.cpp
//  Base class of objects which cannot exist without
// a TCL Intpreter to provide them with services.
// The usual function of the TCL interpreter object is
// to provide Tcl interpreter pointers which can then
// be passed in to Tcl_* functions.

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


#include "TCLInterpreterObject.h"                               


using namespace std;

// Functions for class CTCLInterpreterObject

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLInterpreter* Bind ( CTCLInterpreter* pBinding )
//  Operation Type:
//     Connection
//
CTCLInterpreter* 
CTCLInterpreterObject::Bind(CTCLInterpreter* pBinding) 
{
// Binds the object to a TCL interpreter object.
// The prior object is returned, or NULL if there was
// no prior binding.
// 
// Formal Parameters:
//    CTCLInterpreter* pBinding:
//       New binding to apply to the interpreter.
// Exceptions:  

  CTCLInterpreter*  pPrior = m_pInterpreter;
  m_pInterpreter =  pBinding;
  return            pPrior;
}
