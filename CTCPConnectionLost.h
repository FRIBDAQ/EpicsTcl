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



//
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
#ifndef __CTCPCONNECTIONLOST_H
#define __CTCPCONNECTIONLOST_H

#ifndef __ERRNOEXCEPTION_H
#include <ErrnoException.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward class references:

class CSocket;

/*!
  Encapsulate the connection lost exception.  This exception is thrown
  when a read is attempted on a busted socket.
  */
class CTCPConnectionLost : public CErrnoException
{
  // Private data members:

  STD(string) m_Host;
  STD(string) m_Port;

  mutable STD(string) m_ReasonText;

  // Constructors and related members:

public:
  CTCPConnectionLost(CSocket* pSocket, const char* pDoing);
  CTCPConnectionLost(const CTCPConnectionLost& rhs);
  virtual ~CTCPConnectionLost(){}

  CTCPConnectionLost& operator=(const CTCPConnectionLost& rhs);
  int                 operator==(const CTCPConnectionLost& rhs);

  // Selectors:
  

public:
  STD(string) getHost() const {
    return m_Host; }
  STD(string) getPort() const {
    return m_Port; }

  // Mutators:

protected:
  void setHost(const STD(string)& rHost) {
    m_Host = rHost;
  }
  void setPort(const STD(string)& rPort) {
    m_Port = rPort;
  }

  // Object operations:

public:
  virtual const char* ReasonText() const;

  // Utility functions:

protected:
  void Host(CSocket* pSocket);
  void Port(CSocket* pSocket);

};

#endif
