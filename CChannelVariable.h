#ifndef __CCHANNELVARIABLE_H
#define __CCHANNELVARIABLE_H

/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __TCLVARIABLE_H
#include <TCLVariable.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CTCLInterpreter;
class CChannel;

/*!
   This class is a Tcl variable that represents an Epics channel.
   Normally this will be used by  CTCLChannelCommand.  That class
   will be responsible for setting us to keep us up to date with
   the epics current value.  We catch traces that are used
   to change the value of the EPICS channel. e.g. we want

   set linkedvar value

   to set the corresponding Epics process variable to value.

   In order to prevent nasty traces from firing as epics updates the
   variable.  The CTCLChannelCommand uses setTracing to turn off/on
   tracing when it updates us as a result of an epics channel update.

*/

class CChannelVariable : public CTCLVariable
{
  CChannel*     m_pChannel;	  /* Channel we're linked to. */
  unsigned int  m_referenceCount; /* Number of links.         */
public:
  CChannelVariable(CTCLInterpreter& rInterp,
		   std::string      name,
		   CChannel*        pChannel);
  CChannelVariable(const CChannelVariable& rhs);
  ~CChannelVariable();
  CChannelVariable& operator=(const CChannelVariable& rhs);
  int operator==(const CChannelVariable& rhs) const;
  int operator!=(const CChannelVariable& rhs) const;

public:
  void enableTracing();
  void disableTracing();

  void newEpicsValue(std::string value);

  // The function called on trace:

  virtual char* operator()(char* pName,
			   char* pSubscript,
			   int   flags);
  void Reference();
  unsigned int Dereference();
			   

};



#endif
