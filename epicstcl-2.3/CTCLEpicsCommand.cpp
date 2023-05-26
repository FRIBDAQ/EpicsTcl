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

// #include <config.h>
#include "CTCLEpicsCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include "CTCLChannelCommand.h"

#ifndef __HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static class level data:

CTCLEpicsCommand::KnownChannels CTCLEpicsCommand::m_channelInfo;

///////////////////////////////////////////////////////////////////////////////
/*!
  Construct the command.. nothing much to it.
*/
CTCLEpicsCommand::CTCLEpicsCommand(CTCLInterpreter& interp, string command) :
  CTCLObjectProcessor(interp, command)
{
}
///////////////////////////////////////////////////////////////////////////////
/*!
  Same for destruction.
*/
CTCLEpicsCommand::~CTCLEpicsCommand() {}
///////////////////////////////////////////////////////////////////////////////
/*!
  Even the function call operator just needs to be sure there's a name
  present.
*/
int
CTCLEpicsCommand::operator()(CTCLInterpreter& interp,
			     vector<CTCLObject>& objv)
{
  if (objv.size() != 2) {
    string result = getName();
    result       += " -- incorrect number of parameters\n";
    result       += "Usage:\n";
    result       += "   epicschannel name\n";
    result       += "      name - the name of an epics channel or data record field\n";
    interp.setResult(result);

    return TCL_OK;
  }

  std::string channelName(objv[1]);

  RefCountedChannel* pExisting = haveChannel(channelName);

  // If necessary make a new one:

  if (!pExisting) {
    CTCLChannelCommand* pChannel = new CTCLChannelCommand(interp, string(objv[1]));
    RefCountedChannel newChan(pChannel);
    m_channelInfo[channelName] = newChan;
    pExisting = haveChannel(channelName); // must exist since we just made it!!!
  }
  pExisting->s_refcount++;


  return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////////////
/*
   Find out if we already have a channel.  If so, return a pointer to its
   info record.  If not, then return NULL
*/
CTCLEpicsCommand::RefCountedChannel*
CTCLEpicsCommand::haveChannel(string name)
{
  KnownChannels::iterator i = m_channelInfo.find(name);
  if (i == m_channelInfo.end()) {
    return static_cast<CTCLEpicsCommand::RefCountedChannel*>(NULL);
  }
  else {
    return &(i->second);
  }
}
/////////////////////////////////////////////////////////////////////////////////
/*
   Delete a named channel.  This only does something if decrementing the
   channel's reference count gives a 0.  Note that if the channel does not
   exist, this is a no-op.
*/
void
CTCLEpicsCommand::deleteChannel(string name)
{
  RefCountedChannel*  pInfo = haveChannel(name);
  if (pInfo) {
    pInfo->s_refcount--;
    if(!pInfo->s_refcount) {
      // Delete the channel; remove from the map.

      KnownChannels::iterator i = m_channelInfo.find(name);
      m_channelInfo.erase(i);
      delete pInfo->s_pChannel;

    }
  }
}
///////////////////////////////////////////////////////////////////////////
/*!
    Return a begin iteration iterator for the list of known channels.
*/
CTCLEpicsCommand::KnownChannels::iterator
CTCLEpicsCommand::begin()
{
  return m_channelInfo.begin();
}

/*!
   Return an end iteration iterator for the list of known channels
*/

CTCLEpicsCommand::KnownChannels::iterator
CTCLEpicsCommand::end()
{
  return m_channelInfo.end();
}
