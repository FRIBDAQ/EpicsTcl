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


#include <CChannelVariable.h>
#include <CChannel.h>


using namespace std;


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
  m_pChannel(pChannel),
  m_referenceCount(1)		// Start referenced after all...
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
  Destruction should ensure tracing is off else writes to the channel
  will cause segfaults most likely.
*/
CChannelVariable::~CChannelVariable()
{
  disableTracing();
  // Debugging:


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
   return (char*)NULL;
}


/*!
   Add another reference to the object:
*/
void
CChannelVariable::Reference()
{
  m_referenceCount++;
}
/*!
   Remove a reference and return the resulting reference count.
*/
unsigned int
CChannelVariable::Dereference()
{
  return --m_referenceCount;
}
