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

#ifndef __CTCPCONNECTIONFAILED_H
#define __CTCPCONNECTIONFAILED_H

#ifndef __ERRNOEXCEPTION_H
#include <ErrnoException.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

/*!
   Encapsulates a connection failure exception. Since connect(2) reports
   failure reasons through errno, this classs derives from CErrnoException.
   */
class CTCPConnectionFailed : public CErrnoException
{
  // Private member data:

  STD(string) m_Host;		//!< Attempted peername.
  STD(string) m_Service;		//!< Attempted connection point port. 
  mutable STD(string) m_ReasonText;	//!< Reason text is built up here.

  // Constructors and related functions.

public:
  CTCPConnectionFailed(const STD(string)& host,
		       const STD(string)& service,
		       const char*   pDoing);
  CTCPConnectionFailed(const CTCPConnectionFailed& rhs);
  ~CTCPConnectionFailed() {}	//!< Destructor.

  CTCPConnectionFailed& operator=(const CTCPConnectionFailed& rhs);
  int                   operator==(const CTCPConnectionFailed& rhs);

  // Selectors:

public:
  STD(string) getHost() const
  { return m_Host; }
  STD(string) getService() const
  { return m_Service; }
 
  // Mutators:

protected:
  void setHost(const STD(string)& rHost) 
  { m_Host = rHost; }
  void setService(const STD(string)& rService)
  { m_Service = rService; }

  // Class functions:

public:
  virtual const char* ReasonText() const;
  
};
#endif
