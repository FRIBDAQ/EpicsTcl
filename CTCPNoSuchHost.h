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
// Copyright 
//   NSCL All rights reserved.
//
#ifndef __CTCPNOSUCHHOST_H
#define __CTCPNOSUCHHOST_H

// Headers:

#ifndef __EXCEPTION_H
#include <Exception.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

/*!
   Encapsulates an exception which indicates a failure to translate a host
   specification string into an IP address.  Hosts are translated from two
   posible string formats

   - Hostnames DNS hostnames such as crusher.nscl.msu.edu
   - IPaddresses stringified dotted ip addresses e.g. 35.8.33.224

   The exception produces reason strings which describe why the host could
   not be opened along with contextual information supplied by the application
   when the object is instantiated.
   */
class CTCPNoSuchHost : public CException
{
  // Member data (private):

  int     m_hErrno;		//!< Cached value of errno when instantiated.
  STD(string)    m_Host;	//!< Host which doesn't exist.
  mutable STD(string)  m_Reason;	//!< String for building up reason text.

public:
  // Constructors and related functions:

  CTCPNoSuchHost(const STD(string)& hostname, const STD(string)& Doing);
  CTCPNoSuchHost(const CTCPNoSuchHost& rhs); //!< Copy constructor.
  virtual ~CTCPNoSuchHost() {}	             //!< Destructor.

  CTCPNoSuchHost& operator=(const CTCPNoSuchHost& rhs);
  int             operator==(const CTCPNoSuchHost& rhs);

  // Selectors:

public:
  int gethErrno() const		//!< Return h_errno at time of instantiation
  { return m_hErrno; }
  STD(string) getHost() const	//!< Return name of failing host
  { return m_Host; }
 
  // Mutators:

protected:
  void sethErrno(int newVal)
  { m_hErrno = newVal; }
  void setHost(const STD(string) newVal)
  { m_Host = newVal; }

  // Operations on the class:

public:
  virtual const char *  ReasonText () const;
  virtual Int_t         ReasonCode () const;

};

#endif
