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

   

//! \class: CApplication           
// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

/*!
  \class CApplication
This singleton class is the type of the application object.
main() produces one of these and calls it's operator() function
to start the application.

*/

#ifndef __CAPPLICATION_H  //Required for current class
#define __CAPPLICATION_H

//
// Include files:
//

#ifndef __CSOCKET_H
#include <CSocket.h>
#endif

#ifndef __CCHANNELLIST_H
#include <CChannelList.h>
#endif

#ifndef __STL_STRING
#include <string>        //Required for include files 
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __ISTREAM_DAQH
#include <Istream.h>
#endif

#ifndef __CRT_TIME_H
#include <time.h>
#define __CRT_TIME_H
#endif

// Forward class definitions:

class CBuildChannelData;
struct gengetopt_args_info;
 
class CApplication      
{
  friend class ApplicationTests; // Required for unit test.
private:
  

  STD(string)   m_sSetupFile;  //!<  Name of channel file '-' if stdin.  
  int      m_nPort;       //!<  TCP port of Tcl server we connect to.  
  STD(string)   m_sHost;       //!<  TCP server we connect to.
  int      m_nInterval;   //!<  Seconds between channel updates.
 
  // Class assocations.

  CSocket      m_Socket;   //!< Socket object on tclserver.
  CChannelList m_Channels; //!< List of channels we are maintaining.


public:


  CApplication ();
  ~CApplication ( ); 

  // Singletons dont copy and applications are singletons:

private:
  CApplication(const CApplication& rhs);
  CApplication& operator=(const CApplication& rhs);
  int operator==(const CApplication& rhs) const;
  int operator!=(const CApplication& rhs) const; 
public:


  // Class operations:

public:

  int operator()       (gengetopt_args_info& Parameters)   ; //!< Entry point.
  void ReadChannelFile (STD(istream)& Input)   ;                  //!< Create channel list.
  void Update          ()   ;                                //!< Do an update. 
  void ChannelsToServer(CBuildChannelData& chans)   ;        //!< Send channel info -> server. 
  void ConnectToServer (int nRetryInterval, int nNumRetries)   ; //!< Connect with tcl server.
  //
  // internal utilities.
private:
  void ValidatePort(int port);	// Throw if port is not valid.
  void ValidateInterval(int interval);
  STD(string) FormatTime(time_t t);
  STD(string) GenerateSet(const STD(string)& arrayname, const STD(string)& index, 
		     const STD(string)& value);
  void   Delay(int seconds);
};

#endif
