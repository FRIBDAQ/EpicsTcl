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


#include <CTCLChannelCommand.h>
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include "TCLVariable.h"
#include "CChannelVariable.h"
#include "CChannel.h"
#include "CTCLEpicsPackage.h"
#include "CTCLEpicsCommand.h"

#include <iostream>

#include <set>
#include <string.h>



using namespace std;


/*!
   Construct a channel command.
   \param interp  : CTCLInterpreter&
       The interpreter on which the command is registered.
   \param name    : std::string
       The name of both the channel and the command.
       Note that the name could also be the field-name of a device record.
       For example, to get the units (engineering units) of a device
       named 'george' you would ask for george.EGU.
*/
CTCLChannelCommand::CTCLChannelCommand(CTCLInterpreter& interp,
				       string          name) :
  CTCLObjectProcessor(interp, name),
  m_pChannel(0)
{
  m_pChannel = new CChannel(name);
  m_pChannel->Connect();
  m_interpreterThread = Tcl_GetCurrentThread();
}
/*!
   Destroy a channel command and the associated channel resources:
*/
CTCLChannelCommand::~CTCLChannelCommand()
{

  delete m_pChannel;
  m_pChannel = 0;

  // Empty out the list of linked variables...deleting the variable objects.

  while(!m_linkedVariables.empty()) {
    VariableInfo info = *(m_linkedVariables.begin());
    delete       info.m_pLinkedVar;
    m_linkedVariables.pop_front();
  }
  
}

/*!
   Called when the command for the channel has been issued.  The channel command
   is an ensemble (see the class level comments in the header and the
   comments for each of the utility functions that implements the commands.
   \param interp : CTCLInterpreter&
      Reference to the interpreter that is executing the command.
   \param objv   : vector<CTCLObject>& 
      Reference to a vector of OO encapsulated Tcl_Object*s  that 
      are the command words.
*/
int
CTCLChannelCommand::operator()(CTCLInterpreter&    interp,
			       vector<CTCLObject>& objv)
{
  // Must have at least the subcommand of the ensemble:

  if (objv.size() < 2) {
    string result = objv[0];
    result       += " -- incorrect number of command parameters\n";
    result       += Usage();
    interp.setResult(result);

    return TCL_ERROR;
  }

  // Branch out based on the sub command:

  string subcommand = objv[1];
  if (subcommand == string("get")) {
    return Get(interp, objv);
  }
  else if (subcommand == string("set")) {
    return Set(interp, objv);
  }
  else if (subcommand == string ("delete")) {
    return Delete(interp);
  }
  else if (subcommand == string("updatetime")) {
    return Updatetime(interp);
  }
  else if (subcommand == string("link")) {
    return Link(interp, objv);
  }
  else if (subcommand == string("unlink")) {
    return Unlink(interp, objv);
  }
  else if (subcommand == string("listlinks")) {
    return ListLinks(interp, objv);
  }
  else if (subcommand == string("values")) {
	  return ValueList(interp, objv);
  }
  else if (subcommand == string("size")) {
	  return Size(interp, objv);
  }
  else {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " -- Bad subcommand for channel command\n";
    result       += Usage();
    interp.setResult(result);

    return TCL_ERROR;
  }
}

/////////////////////////////////////////////////////////////////
//////////////////// Utility functions //////////////////////////
/////////////////////////////////////////////////////////////////

/*!
   Get the value of a channel and return it in the result.
   Errors:
   - Channel is not connected at this time.
   Note that the CChannel class is structured so that all gets are a string
   which fits in well with Tcl's EIAS model.
   \param interp : CTCLInterpreter&
      Interpreter executing the command.
   \param objv :  Vector of encapsulated objects that represent the
   					command line.
*/
int
CTCLChannelCommand::Get(CTCLInterpreter& interp,
						vector<CTCLObject>& objv)
{
  if(objv.size() > 3) {
	  string error=("Too many command line parameters\n");
	  error += Usage();
	  interp.setResult(error);
	  return TCL_ERROR;
  }
  // Figure out how many elements we'll ask for...
  
  size_t max = 0;
  if (objv.size() ==3) {
	  try {
		  objv[2].Bind(interp);
		  max = (int)objv[2];
	  }
	  catch(...) {
		  string error("Size parameter must convert to an integer");
		  return TCL_ERROR;
	  }
  }
  

  if (!m_pChannel->isConnected()) {
    string result = m_pChannel->getName();
    result       += " is not connected at this time";
    interp.setResult(result);

    return TCL_ERROR;
  }
  vector<string> data = m_pChannel->getVector(max);
  CTCLObject result;
  result.Bind(interp);
  for (int i=0; i < data.size(); i++) {
	  result += data[i];
  }
  
  interp.setResult(result);
  return TCL_OK;
}
/*!
   Set the value of an epics channel.  The format of the set command is

   channelname set value ?type?

   where the optional type parameter is the data type (string, int, real)
   of the channel.  string in general will work. Errors include:
   - channel not connected
   - incorrect number of parameters.
   - The 'value' is not representabler in the desired type.
   \param interp : CTCLInterpreter&
      Reference to the interpreter that is running this command.
   \param objv : vector<CTCLObject>&
      Reference to the vector of OO Wrapped Tcl_Obj*s that make up the
      command.
*/
int
CTCLChannelCommand::Set(CTCLInterpreter&    interp, 
			vector<CTCLObject>& objv)
{
  string datatype("string");	// default data type.

  // check the number of parameters.

  if ((objv.size() < 3) || (objv.size() > 4)) {
    string result = objv[0];
    result       += " ";
    result       += string(objv[1]);
    result       += " -- insufficient parameters\n";
    result       += Usage();
    interp.setResult(result);

    return TCL_ERROR;
  }

  // Get the value string and, if there is a datatype string update the
  // datatype variable contents:

  string value = objv[2];
  if (objv.size() == 4) {
    datatype = string(objv[3]);
  }

  // The rest of this is inside a try block so that any conversion errors
  // are caught and turned into normal Tcl errors.

  try {
    if (datatype == string("string")) {
    	objv[2].Bind(interp);
    	vector<string> items;
    	int itemCount = objv[2].llength();
    	for (int i =0; i < itemCount; i++) {
    		items.push_back((string)(objv[2].lindex(i)));
    	}
    	(*m_pChannel)  = items;  // Vector set.
    }
    else if (datatype == string("int")) {
      objv[2].Bind(interp);
      vector<int> items;
      int itemCount = objv[2].llength();
      for(int i=0; i < itemCount; i++) {
    	  items.push_back(objv[2].lindex(i));
      }
      
      (*m_pChannel) = items;
    }
    else if (datatype == string("real")) {
      objv[2].Bind(interp);
      vector<double> items;
      int itemCount = objv[2].llength();
      for (int i =0; i < itemCount; i++) {
    	items.push_back(objv[2].lindex(i));  
      }
      
      (*m_pChannel) = items;
    }
    else {
      string result = objv[0];
      result += " ";
      result += string(objv[1]);
      result += " ";
      result += string(objv[2]);
      result += " ";
      result += datatype;
      result += " -- invalid data type\n";
      result += Usage();
      interp.setResult(result);

      return TCL_ERROR;
    }
  }
  catch (...) {
    string result = objv[0];
    result       += " ";
    result       +=string(objv[1]);
    result       += " ";
    result       += string(objv[2]);
    result       += " ";
    result       += datatype;
    result       += "\n -- data does not all convert to selecte data type \n";
    result       += "or is not a proper list.\n";
    interp.setResult(result);

    return TCL_ERROR;

  }
  return TCL_OK;
}
/*!
   Get the time at which the channel was last updated.
   This will be the epoch if the channel has never been updated.
   This value is returned in a form suitable for use with [clock format]
   \param interp : CTCLInterpreter&
     References the interpreter on which this command is running

*/
int
CTCLChannelCommand::Updatetime(CTCLInterpreter& interp)
{
  time_t Time = m_pChannel->getLastUpdate();
  CTCLObject result;
  result.Bind(interp);
  result = (int)Time;
  interp.setResult(result);
  return TCL_OK;
  
}

/*!
  Delete the command.
*/
int
CTCLChannelCommand::Delete(CTCLInterpreter& interp)
{
  CTCLEpicsCommand::deleteChannel(getName());
  return TCL_OK;
}

/*!
   Link a variable to the channel.  This is sort of like many
   Tk widget's -textvariable option  The variable is assumed to
   specify a global variable.
*/
int
CTCLChannelCommand::Link(CTCLInterpreter& interp,
			 vector<CTCLObject>& objv)
{


  // Need a variable name:

  if (objv.size() != 3) {
    string result = getName();
    result       += " - insufficient parameters\n";
    result       += Usage();
    interp.setResult(result);

    return TCL_ERROR;
  }
  string tclVarName = objv[2];


  // If the variable is linked to another channel that's an error:

  if (isLinked(tclVarName)) {
    string result = tclVarName;
    result       += " is already linked to a different channel";
    interp.setResult(result);
    return TCL_ERROR;
  }

  // If a variable with the same name already exists, simply
  // add another reference to it...otherwise, make a new one
  // and register it... ensure that we have a trace handler established
  // for this channel as well so we can queue update events to 
  // the tcl interpreter thread:

  VariableInfoIterator i = find(tclVarName);
  if (i != m_linkedVariables.end()) {
    CChannelVariable* pLinkedVariable = i->m_pLinkedVar;
    pLinkedVariable->Reference();
  }
  else {

    // Variable does not yet have a link

    CChannelVariable* pLinkedVar = new CChannelVariable(interp, tclVarName, m_pChannel);
    pLinkedVar->newEpicsValue(m_pChannel->getValue().c_str());
    VariableInfo vinfo = {tclVarName, pLinkedVar};
    m_linkedVariables.push_back(vinfo);

  }    
  // This may be redundant.. maybe not.. but it does not hurt to do this
  // more than once..
  
  m_pChannel->setSlot(CTCLChannelCommand::markChange, this);
  
  return TCL_OK;
}

/*!
  Unlink a variable from the channel.
*/
int
CTCLChannelCommand::Unlink(CTCLInterpreter& interp,
			   vector<CTCLObject>& objv)
{
  // We need a single parameter, the variable name to unlink:
  //
  if (objv.size() != 3) {
    string result = "Invalid number of parameers for channel-name unlink\n";
    result += Usage();
    interp.setResult(result);
    return TCL_OK;
  }
  string tclVarName = objv[2];

  // Locate the named variable in the list and dereference it.
  // if the reference count is zero.. then delete and remove it...
  // if that results in an empty list of liniks... turn off
  // our trace handler:

  VariableInfoIterator i = find(tclVarName);

  if (i != m_linkedVariables.end()) {
    CChannelVariable*  pLinkedVariable = i->m_pLinkedVar;
    if (pLinkedVariable->Dereference() == 0) {
      // Last link deleted:

      delete pLinkedVariable;
      m_linkedVariables.erase(i);
      if (m_linkedVariables.empty()) {
	m_pChannel->setSlot(NULL,NULL);
      }
    }
    return TCL_OK;
  }
  else {
    // Could not find linked variable:
    
    string result = "The variable ";
    result  += tclVarName;
    result  += " is not linked to ";
    result  += getName();
    result  += "\n";
    result  += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }


}

/*
   Lists the set of links that have been created. The links that
   match the specified pattern are listed in alphabetical order.
   If no pattern is specified, it defaults, as usual to *


*/
int
CTCLChannelCommand::ListLinks(CTCLInterpreter& interp, 
			      std::vector<CTCLObject>& objv)

{

  // Figure the pattern and validate the command parameter count.

  string pattern = "*";		// Default pattern.
  if(objv.size() > 3) {
    string result = "Too many parametesr to linklist subcommand\n";
    result += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  if (objv.size() == 3) {
    pattern = string(objv[2]);
  }
  // We're going to throw the matching variable names up into a set which
  // will give us a sorted parameter list.

  set<string> sortedList;
  for (VariableInfoIterator i= m_linkedVariables.begin();
      i != m_linkedVariables.end(); i++) {
    string name = i->varname;
    if (Tcl_StringMatch(name.c_str(), pattern.c_str())) {
      sortedList.insert(name);
    }
  }

  // Now we create a list from this sorted thing.

  CTCLObject result;
  result.Bind(interp);

  set<string>::iterator p;
  for(p = sortedList.begin(); p != sortedList.end(); p++) {
    result += *p;		// Does list formatting.
  }
  interp.setResult(result);
  return TCL_OK;

}
/*
  Provide command usage.
*/
string
CTCLChannelCommand::Usage()
{
  string result = "Usage: \n";
  result       += getName();
  result       += "  get ?count?\n";
  result       += getName();
  result       += " set value_list ?string|int|real?\n";
  result       += getName();
  result       += " link tclVariableName\n";
  result       += getName();
  result       += " unlink tclVariableName\n";
  result       += getName();
  result       += " listlinks ?pattern?\n";
  result       += getName();
  result       += " updatetime\n";
  result       += getName();
  result       += " delete\n";
  result       += getName();
  result       += " values\n";
  result       += getName();
  result       += " size\n";


  return result;
}

/*
 *   Called to process the 'values' subcommand.
 *   The result will be a Tcl list of the legal values
 *   the variable can assume.  An empty list will indicate
 *   the value is not yet connected.
 */
int
CTCLChannelCommand::ValueList(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
	if (objv.size() != 2) {
		string result("Too many command line parameters\n");
		result += Usage();
		interp.setResult(result);
		return TCL_ERROR;
	}
	// At this point we can't fail:
	
	vector<string> allowedValues = m_pChannel->getAllowedValues();
	
	CTCLObject result;
	result.Bind(interp);
	for (int i =0; i < allowedValues.size(); i++) {
		result += allowedValues[i];
	}
	interp.setResult(result);
	return TCL_OK;
	
}
/*
 * Return the number of elements the channel contains.
 * An error is returned if the channel is not yet connected.
 * Note that we won't let the channel throw it's exception, but
 * will explicitly check for a connection.
 */
int
CTCLChannelCommand::Size(CTCLInterpreter& interp, 
			 std::vector<CTCLObject>& objv)
{
	// It's an error to pass in any arguments other than
	// the subcommand.
	
	if (objv.size() != 2) {
		string result("Invalid parameter count\n");
		result += Usage();
		interp.setResult(result);
		return TCL_ERROR;
	}
	
	// If the channel is connected we can get the info
	// otherwise it's an error.
	
	if (m_pChannel->isConnected()) {
		CTCLObject result;
		result.Bind(interp);
		result = (int)m_pChannel->size();
		interp.setResult(result);
		return TCL_OK;
	}
	else {
		interp.setResult("Channel not connected");
		return TCL_ERROR;
	}

}

// Updates the values of all linked variables we are maintaining.



void
CTCLChannelCommand::UpdateLinkedVariable()
{

  for (VariableInfoIterator i = m_linkedVariables.begin();
       i != m_linkedVariables.end(); i++) {
    CChannelVariable* pVar = i->m_pLinkedVar;
    pVar->newEpicsValue(m_pChannel->getValue().c_str());

  }

}

/*
  Queue a Tcl event to actually do the update.
*/
void
CTCLChannelCommand::markChange(CChannel* pChannel, void* pObject)
{

  CTCLChannelCommand* command = static_cast<CTCLChannelCommand*>(pObject);

  ChangeEvent* pEvent     = (ChangeEvent*)Tcl_Alloc(sizeof(ChangeEvent));
  pEvent->rawEvent.proc   = CTCLChannelCommand::update;
  pEvent->pChangedChannel = command;
  string name             = command->getName();
  pEvent->channelName     = Tcl_Alloc(strlen(name.c_str()) + 1);
  strcpy(pEvent->channelName, name.c_str());
 

  if (Tcl_GetCurrentThread() == command->m_interpreterThread) {
    // If we are the Tcl thread already, we don't really need to
    // go through the overhead of declaring an event.. so just
    // call the event processor.

    update((Tcl_Event*)pEvent, 0);
    Tcl_Free((char*)pEvent);	// The event proc frees the string, but
                                // we need to free the event itself, as 
                                // Tcl's event dispatching would. 
  } 
  else {
    // If we are called in a different thread, in order to
    // match the 'apartment' model of tcl threading we need to 
    // execute the update handler in the context of the Tcl thread...
    // and therefore must let the Tcl event loop schedule the
    // execution of update.
    // 
    Tcl_ThreadQueueEvent(command->m_interpreterThread, (Tcl_Event*)pEvent,
			 TCL_QUEUE_TAIL);
    Tcl_ThreadAlert(command->m_interpreterThread);
  }
}

/*
   This function is called in the thread of the interpreter after
   an epics variable has changed.
*/
int
CTCLChannelCommand::update(Tcl_Event* p, int flags)
{

  CTCLChannelCommand::ChangeEvent* pEvent = (CTCLChannelCommand::ChangeEvent*)p;

  // If the command is no longer registered, then it was deleted between
  // the event being queued to us and dispatched to us.. so we just
  // ignore the event.. otherwise
  // process it.
  

  if (CTCLEpicsCommand::haveChannel(string(pEvent->channelName))) {
    pEvent->pChangedChannel->UpdateLinkedVariable();
  }
  // Need to release the command name storage:
 
  Tcl_Free(pEvent->channelName);

  return 1;
  
}

/*
   Returns true if there are any channels that have the named TCL variable as a linked
   variable.. false otherwise.
*/
bool
CTCLChannelCommand::isLinked(string variableName)
{
  CTCLEpicsCommand::KnownChannels::iterator i = CTCLEpicsCommand::begin();


  // Remember we can multiply link to ourself, but not to other chans:

  while (i != CTCLEpicsCommand::end()) {
    CTCLChannelCommand* pItem = i->second.s_pChannel;
    if ((pItem->find(variableName) != (pItem->m_linkedVariables).end()) &&
	(pItem != this)) {
      return true;
    }
    i++;
  }
  return false;
}

/*
  Locate a variable named in the linked variable list.
  Parameters:
     variableName - Name of the variable to find.
 
  returns m_linkedVariables.end() if no match.


*/
CTCLChannelCommand::VariableInfoIterator
CTCLChannelCommand::find(string variableName)
{
  VariableInfoIterator i = m_linkedVariables.begin();
  while (i != m_linkedVariables.end()) {
     if(i->varname == variableName) {
       return i;
     }
     i++;
  }
  return i;			// It's end() now.
}
