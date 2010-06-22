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

//  CTCLFileHandler.h:
//
//    This file defines the CTCLFileHandler class.
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

#ifndef __TCLFILEHANDLER_H	//Required for current class
#define __TCLFILEHANDLER_H
				//Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                            

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef _WINDOWS
#ifndef __CRT_UNISTD_H
#include <unistd.h>
#define __CRT_UNISTD_H
#endif
#endif

#ifndef __CRT_STDIO_H
#include <stdio.h>
#define __CRT_STDIO_H
#endif
  

class CTCLFileHandler  : public CTCLInterpreterObject        
{
  UInt_t m_nFid;  // File descriptor associated with the object.
  
public:
			//Default constructor

  CTCLFileHandler (CTCLInterpreterObject* pInterp,  
		   UInt_t am_nFid
#ifndef _WINDOWS
			= STDIN_FILENO
#endif
			) :       

    CTCLInterpreterObject(pInterp->getInterpreter()),
    m_nFid (am_nFid)  
  { }        
  CTCLFileHandler(CTCLInterpreterObject* pInterp,
		  FILE* pFile) :
    CTCLInterpreterObject(pInterp->getInterpreter()),
#ifdef _WINDOWS
	m_nFid(_fileno(pFile))
#else
    m_nFid(fileno(pFile))
#endif
  { }
  CTCLFileHandler(CTCLInterpreter* pInterp,
		   UInt_t am_nFid 
#ifndef _WINDOWS
				= STDIN_FILENO
#endif
				) :       
    CTCLInterpreterObject(pInterp),
    m_nFid(am_nFid)
  { }
  CTCLFileHandler(CTCLInterpreter* pInterp,
		  FILE* pFile) :
    CTCLInterpreterObject(pInterp),
#ifdef _WINDOWS
	m_nFid(_fileno(pFile))
#else
    m_nFid(fileno(pFile))
#endif
  { }
  ~CTCLFileHandler ( ) {Clear(); }       //Destructor	
			//Copy constructor

  CTCLFileHandler (const CTCLFileHandler& aCTCLFileHandler )   : 
    CTCLInterpreterObject (aCTCLFileHandler) 
  {   
    m_nFid = aCTCLFileHandler.m_nFid;
                
  }                                     

			//Operator= Assignment Operator

  CTCLFileHandler& operator= (const CTCLFileHandler& aCTCLFileHandler)
  { 
    if (this == &aCTCLFileHandler) return *this;          
    CTCLInterpreterObject::operator= (aCTCLFileHandler);
    m_nFid = aCTCLFileHandler.m_nFid;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLFileHandler& aCTCLFileHandler) const
  { return (
	    (CTCLInterpreterObject::operator== (aCTCLFileHandler)) &&
	    (m_nFid == aCTCLFileHandler.m_nFid) 
	    );
  }                             
  // Readonly selectors:        

public:
  UInt_t getFid() const
  {
    return m_nFid;
  }
  // Mutators:

public:                       
  void setFid (UInt_t am_nFid)
  { 
    m_nFid = am_nFid;
  }
  // operations and overridables:

public:
  virtual   void operator() (int mask)   = 0;
  static  void CallbackRelay (ClientData pObject, int mask)  ;  
  void Set (int mask)  ;
  void Clear ()  ;
  

};

#endif
