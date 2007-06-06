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

#ifndef __CTCLEPICSCOMMAND_H
#define __CTCLEPICSCOMMAND_H

#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

class CTCLInterpeter;
class CTCLObject;
class CTCLChannelCommand;

/*!
    The epicschannel command provides a mechanisms for creating objects
    of type CTCLChannelCommand... that is for creating a command that
    accesses Epics channels or epics channel data record entries.

    The syntax of this command is trivial:

    epicschannel  name

    where name is the name of both the channel and command for the channel
    The command returns the name string on success.
*/
class CTCLEpicsCommand  : public CTCLObjectProcessor
{
private:
  // This structure is used to manage all the epics channel commands
  // and their reference counts.  This is done so that sections of a large
  // application can create/delete channels without any fear of stepping on the
  // toes of other parts of the same application that might be creatign and
  // deleting the same channels.  The idea is that creating an existing channel
  // will just increment its refcount, deleting an existing channel will
  // decrement its refcount and not delete the actual channel object until
  // the refcount is 0.

  struct RefCountedChannel {
    int                 s_refcount;
    CTCLChannelCommand* s_pChannel;
    RefCountedChannel(CTCLChannelCommand* p) :
      s_refcount(0), s_pChannel(p) {}
    RefCountedChannel() :
      s_refcount(0), s_pChannel(0) {}
    RefCountedChannel& operator=(const RefCountedChannel& rhs) 
    {
      s_refcount = rhs.s_refcount;
      s_pChannel = rhs.s_pChannel;
      return *this;
    }
  };
  // The map type below keeps track of all the channels in a by-name lookup 
  // structure:

  typedef std::map<std::string, RefCountedChannel> KnownChannels;

  // class level data:

  static KnownChannels  m_channelInfo;
public:
  CTCLEpicsCommand(CTCLInterpreter& interp, std::string command = std::string("epicschannel"));
  virtual ~CTCLEpicsCommand();
private:
  CTCLEpicsCommand(const CTCLEpicsCommand& rhs);
  CTCLEpicsCommand& operator=(const CTCLEpicsCommand& rhs);
  int operator==(const CTCLEpicsCommand& rhs) const;
  int operator!=(const CTCLEpicsCommand& rhs) const;

public:


  virtual int operator()(CTCLInterpreter& interp,
			 std::vector<CTCLObject>& objv);

  // class level functions:

  static void                deleteChannel(std::string name);
  static RefCountedChannel*  haveChannel(std::string name);
 
};


#endif
