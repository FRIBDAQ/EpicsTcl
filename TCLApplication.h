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

//  CTCLApplication.h:
//
//    This file defines the CTCLApplication class.
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

#ifndef __TCLAPPLICATION_H  //Required for current class
#define __TCLAPPLICATION_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                               


                               
class CTCLApplication  : public CTCLInterpreterObject        
{
  void* m_pInit;	  // Ensure app init pulled out of link.
public:
			//Default constructor

  CTCLApplication ()   {m_pInit = (void*)Tcl_AppInit; } // Ensure TCLApplication.o
				                  // is linked.
  ~CTCLApplication ( ) { }       //Destructor

	
			//Copy constructor

  CTCLApplication (const CTCLApplication& aCTCLApplication )   : 
    CTCLInterpreterObject (aCTCLApplication) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CTCLApplication& operator= (const CTCLApplication& aCTCLApplication)
  { 
    if (this == &aCTCLApplication) return *this;          
    CTCLInterpreterObject::operator= (aCTCLApplication);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLApplication& aCTCLApplication)
  { 
    return (
	    (CTCLInterpreterObject::operator== (aCTCLApplication))

	    );
  }                             
                       
  virtual   int operator() ()  =0;
 
};

#endif
