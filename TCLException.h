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

//  CTCLException.h:
//
//    This file defines the CTCLException class.
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

#ifndef __CTCLEXCEPTION_H  //Required for current class
#define __CTCLEXCEPTION_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif

#ifndef __TCLRESULT_H
#include "TCLResult.h"
#endif

                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif  
                           
#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

                               
class CTCLException  : public CTCLInterpreterObject ,public CException
{
   Int_t m_nReason;  // Reason the exception was thrown
				// TCL_ERROR    - Error in script.
				// TCL_BREAK    - Break from loop.
				// TCL_CONTINUE - continue loop.
				// TCL_RETURN   - fuction return.
				// NOTE: Really no business throwing anything
				//       but TCL_ERRORs.
   std::string m_ResultText;		// Cached result text at construction time.
public:
			//Default constructor

  CTCLException (CTCLInterpreter& am_rInterpreter,  
		 Int_t am_nReason,
		 const char* pString) :
    CTCLInterpreterObject(&am_rInterpreter), 
    CException(pString),
    m_nReason(am_nReason)
  {
    m_ResultText = std::string(GetResult());
  }
  CTCLException(CTCLInterpreter& am_rInterpreter,
		Int_t am_nReason,
		const std::string& rString) : 
    CTCLInterpreterObject(&am_rInterpreter),
    CException(rString),
    m_nReason(am_nReason)
  {
    m_ResultText = std::string(GetResult());
  }
  virtual ~CTCLException ( ) { }       //Destructor
	
			//Copy constructor

  CTCLException (const CTCLException& aCTCLException )   : 
    CTCLInterpreterObject (aCTCLException),   
    CException (aCTCLException) 
  {   
    m_nReason = aCTCLException.m_nReason;            
    m_ResultText = aCTCLException.m_ResultText;
  }                                     

			//Operator= Assignment Operator

  CTCLException operator= (const CTCLException& aCTCLException)
  { 
    if (this == &aCTCLException) return *this;          
    CTCLInterpreterObject::operator= (aCTCLException);
    CException::operator= (aCTCLException);

    if(this != &aCTCLException) {
      m_nReason = aCTCLException.m_nReason;
      m_ResultText = aCTCLException.m_ResultText;
    }
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLException& aCTCLException)
  { return (
	    (CTCLInterpreterObject::operator== (aCTCLException)) &&
	    (CException::operator== (aCTCLException)) &&
	    (m_nReason == aCTCLException.m_nReason) 
	    );
  }                  
  // Selectors:  - Stick to generic CException and CTCLInterpreterObject
  //               interfaces unless you really know what you're doing.
  //
public:

  Int_t getReason() const
  {
    return m_nReason;
  }
  //
  //  Mutators:   Available to derivce classes only:
  //
protected:
                       
          
  void setInterpreter (CTCLInterpreter& am_rInterpreter)
  { 
    Bind(am_rInterpreter);
  }
  void setReason (Int_t am_nReason)
  { 
    m_nReason = am_nReason;
  }
  // TCL Specific interface:
  //
public:
  void AddErrorInfo (const char* pMessage)  ;
  void AddErrorInfo(const std::string& rMessage) {
    AddErrorInfo(rMessage.c_str());
  }
  void AddErrorInfo(const CTCLString& rMessage) {
    AddErrorInfo((const char*)(rMessage));
  }

  void SetErrorCode (const char* pMessage, 
		     const char* pMnemonic="???", 
		     const char* pFacility="TCL", 
		     const char* pSeverity="FATAL")  ;
  void SetErrorCode(const std::string rMessage,
		    const std::string &rMnemonic=std::string("???"),
		    const std::string &rFacility=std::string("TCL"),
		    const std::string &rSeverity=std::string("FATAL")) {
    SetErrorCode(rMessage.c_str(), rMnemonic.c_str(),
		 rFacility.c_str(), rSeverity.c_str());
  }

  //
  // CException generic interface:
  //
  virtual   const char* ReasonText () const;
  virtual   Int_t ReasonCode () const  ;
private:
  CTCLResult GetResult ();
  
};

#endif
