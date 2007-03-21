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

#include <config.h>
#include <CChannelVariable.h>
#include <CChannel.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
    Create a CChannelVariable - this is a Tcl variable that
    implements tracing on writes.
    \param rInterp  - Reference to the interpreter on which the variable is defined.
    \param name     - Name of the variable (could be an array element).
    \param pChannel - Points to the Epics channel that we are linked to.

*/
CChannelVariable::CChannelVariable(CTCLInterpreter& rInterp, 
				   string           name,
				   CChannel*        pChannel) :
  CTCLVariable(&rInterp, name, false),
  m_pChannel(pChannel)
{
  // now turn on tracing the way we want it:

  enableTracing();
}

/*!
   Copy construction - about all we can do is base class copy construction
   and a shallow copy of CChannel:
*/
CChannelVariable::CChannelVariable(const CChannelVariable& rhs) :
  CTCLVariable(rhs)
{
  m_pChannel = rhs.m_pChannel;
}
/*!
   Assignment.. this is really re-defining the object to be a different
   variable, not assigning a value to it:
*/
CChannelVariable&
CChannelVariable::operator=(const CChannelVariable& rhs)
{
  if (&rhs != this) {
    CTCLVariable::operator=(rhs);
    m_pChannel = rhs.m_pChannel;
  }
  return *this;
}
/*!
   Equality comparison.. base class and require the channels to be the same.
*/
int
CChannelVariable::operator==(const CChannelVariable& rhs) const
{
  return (CTCLVariable::operator==(rhs)    &&
	  (m_pChannel == rhs.m_pChannel));
}
/*!
  Inequality is the logical inverse of equality:
*/
int
CChannelVariable::operator!=(const CChannelVariable& rhs) const
{
  return !(*this == rhs);
}



/*!
   Enable the right kind of tracing... in this case
   we want to only trace Writes.
*/
void
CChannelVariable::enableTracing()
{
  Trace(TCL_TRACE_WRITES);
}
/*!
  Disable tracing - used to prevent a trace from firing when
  epics updates the value
*/
void
CChannelVariable::disableTracing()
{
  UnTrace();
}

/*!
  Provide a new value from epics.. this is done with tracing turned off.
  otherwise we can get an infinite trace loop: we set the value,
  the trace fires, which causes us to set the epics channel which causes
  epics to update us ....etc.

  \param value - New string value of the epics channel.
*/
void
CChannelVariable::newEpicsValue(string value)
{
  disableTracing();
  Set(value.c_str());
  enableTracing();
}

/*!
   Trace function. This is called when a script or C/C++ code
   updates the value of the variable and tracing is enabled.
   We must set the channel to the new value:
   Parameters are ignored.. must return NULL.
*/
char*
CChannelVariable::operator()(char* name, char* subscript, int flags)
{
  (*m_pChannel) = Get();
}
