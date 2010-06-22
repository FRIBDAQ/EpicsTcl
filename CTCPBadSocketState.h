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

// Author
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
//   NSCL All rights reserved.
//
#ifndef __CTPBADSOCKETSTATE_H
#define __CTPBADSOCKETSTATE_H

// Headers:

#ifndef __EXCEPTION_H
#include <Exception.h>
#endif

#ifndef __CSOCKET_H
#include <CSocket.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

/*!
  Encapsulates an exception which will be thrown whenever a CSocket member
  is called when the socket is in an invalid state.  The exception
  recognizes that there may be a list of valid states which the socket
  can be in and will indicate in the exception message the set of valid
  states.
  */
class CTCPBadSocketState : public CException
{
  // Private members:
  
  CSocket::State          m_BadState;	 //!< Incorrect state at time of throw.
  STD(vector)<CSocket::State>  m_ValidStates; //!< States which would have been ok.
  mutable STD(string)          m_Message;     //!< Full error message built up here

  // Constructors and related functions:
public:
  CTCPBadSocketState(CSocket::State badState,
		     STD(vector)<CSocket::State> okStates,
		     const char* pDoing);
  CTCPBadSocketState(const CTCPBadSocketState& rhs);
  virtual ~CTCPBadSocketState() {}

  CTCPBadSocketState& operator=(const CTCPBadSocketState& rhs);
  int                 operator==(const CTCPBadSocketState& rhs);


  // Selectors:
public:
  CSocket::State getBadState() const 
  { return m_BadState; }
  STD(vector)<CSocket::State> getValidStates() const
  { return m_ValidStates; }

  // Mutators:
protected:

  void setBadState(CSocket::State newState)
  { m_BadState = newState; }
  void setValidStates(const STD(vector)<CSocket::State>& newStates)
  { m_ValidStates = newStates; }

  // Operations on the class:

public:
  virtual const char* ReasonText() const;
    
};

#endif
