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
#include "CChannel.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


#ifndef _WINDOWS
#ifdef Linux
#define MUTEX_TYPE PTHREAD_MUTEX_RECURSIVE_NP
#else
#define MUTEX_TYPE PTHREAD_MUTEX_RECURSIVE
#endif
#endif

#ifdef _WINDOWS
#define dbr_buffersize(typename, count) (sizeof(typename)*(count))
#endif

using namespace std;




static void
nullConnectionHandler(connection_handler_args arg)
{
  
}

/**
 * Construct a channel.  This member function 
 * initializes the data associated with a channel, but does not actually
 * connect it to EPICS.  See the Connect function to do that.
 *
 * @param name
 *     Name of the channel.
 *
 * Note that if the channel does not exist, it is not an error: it will just
 * never be marked connected.
 */
CChannel::CChannel(string name) :
  m_sName(name),
  m_fConnected(false),
  m_nChannel(0),
  m_fUpdateHandlerEstablished(false),
  m_fConnectionHandlerEstablished(false),
  m_sValue(""),
  m_LastUpdateTime(0),
  m_pConverter(0),
  m_pHandler(0),
  m_pHandlerData(0)

{
#ifdef _WINDOWS
	InitializeCriticalSection(&m_Monitor);
#else
  pthread_mutexattr_t   attributes;
  
  int status = pthread_mutexattr_init(&attributes);
  status     = pthread_mutexattr_settype(&attributes, MUTEX_TYPE);
  if (status) {
    throw string("BUG - could not set mutex attribute type in CChannel initializer");
  }
  status = pthread_mutex_init(&m_Monitor, &attributes);

  status     = pthread_mutexattr_destroy(&attributes);
#endif
}
/**
 *  Destroys a channel, must cancel all events etc.
 */
CChannel::~CChannel()
{
  CConverter* converter = m_pConverter;
  m_pConverter = 0;		// No update handler is now noop.

  // This holds the lock...
  {
    CCriticalRegion enter(m_Monitor);

    if (m_fConnected || (m_fUpdateHandlerEstablished)) {
      ca_clear_subscription(m_updateEventId);	// Stop event update event dispatching.
    }

    ca_clear_channel(m_nChannel);

  }
  doEvents(0.1);		// Let the events run down.

  // Now kill off the converter and the rest of our stuff.

  {
    CCriticalRegion enter(m_Monitor);
    delete converter;
  }


  // but the mutex must be unlocked to release ...there's a tiny
  // timing hole here.

#ifdef _WINDOWS
  DeleteCriticalSection(&m_Monitor);
#else
  pthread_mutex_destroy(&m_Monitor);
#endif

}

/**
 *   Return the name associated with the channel.
 * 
 * \return string
 * \retval Name of the channel associated with this object.
 */
string
CChannel::getName() const
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...

  return m_sName;
}

/**
 * Tell the caller if this channel is connected to the EPICS event system.
 * \return bool
 * \retval true  - Connected to the system.
 * \retval false - Not connected to the system.
 */
bool
CChannel::isConnected() const
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...
  return m_fConnected;
}

/**
 * Connect the channel to epics.  We do a ca_search_and_connect
 * specifying ourselves as the 'channel data' and a static member
 * as the handler.
 */
void
CChannel::Connect()
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...

  if(!m_fConnectionHandlerEstablished) {
    ca_create_channel(m_sName.c_str(), StateHandler, (void*)this,
		      0, &m_nChannel);
  }
  m_fConnectionHandlerEstablished = true;
}

/**
 * Returns the time at which this channel last received an update.
 */
time_t
CChannel::getLastUpdate() const
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...

  return m_LastUpdateTime;
}

/**
 *  Returns the stringified version of the most recent channel value:
 */
string
CChannel::getValue() const
{
  CCriticalRegion Monitor(m_Monitor);    // Lock value of the string until 
  string value =  m_sValue;              // it's copied for return.
  return value;
}



/*!
 * Return the set of allowed values for the channel.
 * \return std::vector<std::string>
 * \retval empty vector - channel is not yet connected.
 *                        or is an enum that has not yet
 *                        received a value and therefore cannot
 *                        populate its value list.
 * \retval vector size == 1 - Channel is not an enum type.
 * \retval vector size > 1  - Channel is an enum type.
 */

vector<string>
CChannel::getAllowedValues() const
{
	vector<string> result;
	if (!m_pConverter) return result;    // Not connected
	return m_pConverter->allowedValues();
}
/*!
 *  Returns the number of elements in the channel
 *  this will be 1 for a scalar channel, and something
 *  larger than 1 if the channel is a vector.
 *  If the channel is not yet connected, I'm assuming we can't
 *  get that yet so we will throw a string exception:
 *  "Channel not connected"
 */
size_t
CChannel::size() const
{
	if (m_fConnected) {
		return ca_element_count(m_nChannel);
	}
	else {
		throw "Channel not connected";
	}
}
/*!
 *  Returns a vector value channel:
 *  Throws an exception if the channel is not connected.
 *
 *  @param max - maximum number of elements that will be filled:
 *               - 0 - all elements.
 *               - If larger than the vector size the full vector is returned.
 *
 */
std::vector<std::string>
CChannel::getVector(size_t max)
{
  CCriticalRegion Monitor(m_Monitor);
  if (m_fConnected) {
    size_t elements;
    if ((max == 0) || (max > m_VectorValue.size())) {
      elements = m_VectorValue.size();
    } else {
      elements = m_VectorValue.size();
    }

    // The stuff below assumes that either
    // - typically we want all or most of the vector.
    // - or vectors are short:
    //
    std::vector<string>   result = m_VectorValue;

    while(result.size() > elements) {
      result.pop_back();	// Trim the vector from the rear.
    }
    
    return result;
  } else {
    throw "Channel not connected";
  }
}
/*!
  Sets a handler slot for channel value notifications:
  \param handler : CChannel::Slot 
     Function called when the channel value changes. If this is null,
     then notification is disabled.
  \param data : void*
     Data passed as the second parameter of the handler.
     The first parameter of the handler is a pointer to the channel.

*/
void
CChannel::setSlot(CChannel::Slot handler, void* data)
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...

  m_pHandler     = handler;
  m_pHandlerData = data;
}

/*!
   set the channel to a string value..no-op if not connected.
*/
string
CChannel::operator=(string value)
{
  CCriticalRegion Monitor(m_Monitor);   // Probably don't have to but...
  if(m_fConnected) {
    ca_put(DBR_STRING, m_nChannel, const_cast<char*>(value.c_str()));
    ca_flush_io();
  }
  return value;
}
/*!
   Set the channel to an integer value.
*/

int
CChannel::operator=(int value)
{
  CCriticalRegion Monitor(m_Monitor);   // Just in case...
  if (m_fConnected) {
    ca_put(DBR_INT, m_nChannel, &value);
    ca_flush_io();
  }
  return value;
}

/*!
  Set channel to a double  value.
*/
double
CChannel::operator=(double value)
{
  CCriticalRegion Monitor(m_Monitor);   // just in case.
  if (m_fConnected) {
    ca_put(DBR_DOUBLE, m_nChannel, &value);
    ca_flush_io();
  }
  return value;

}

/*!
 *  Set a channel to a vector of strings.
 *  - If the channel is not connected this is does nothing.
 *  - If the vector has too many elements, only the first few
 *    are used.
 *  - If a string of the vector is too long to fit in an dbr_string_t
 *    it is truncated so as to preserve a null terminated string
 *    'case I don't know if epics will handle well if it's dbr_string_t
 *    is not null terminated.
 *  - If buffer cannot be allocated this is a no-op too.
 * 
 * \param rhs   - The vector of strings on the right hand side
 *                of the assigment.
 * \return std::vector<std::string>&  
 * \retval Reference to the source vector.
 */
const vector<string>&
CChannel::operator=(const std::vector<string>& rhs)
{
	
	if (m_fConnected) {
		CCriticalRegion Monitor(m_Monitor);
		
		// Figure out the actual item count:
		
		int count = rhs.size();
		if (count > ca_element_count(m_nChannel)) {
			count = ca_element_count(m_nChannel);
		}
		// Now we need to allocate storage for the array of strings:
#ifdef _WINDOWS
        size_t bytesNeeded      = dbr_buffersize(dbr_string_t, count);
#else
		size_t bytesNeeded      = dbr_size_n(DBR_STRING, count);
#endif
		dbr_string_t* pStorage  = (dbr_string_t*)malloc(bytesNeeded);
		if (pStorage) {
			memset(pStorage, 0, bytesNeeded);
			dbr_string_t*  pDest = pStorage;
			for (int i =0; i < count; i++) {
				strncpy(*pDest, rhs[i].c_str(), sizeof(dbr_string_t)-1);
				pDest++;
			}
			// Set the channel; flush the buffers:
			
			ca_array_put(DBR_STRING, count, m_nChannel, pStorage);
			ca_flush_io();
			free(pStorage);

		}
	}

	return rhs;
}

/*!
 * Set the array channel to a vector of integers.
 * The type chosen will be a DBR_LONG as DBR_INT
K5RGA_HSCAN_DAT * is evidently a 32 bit value.
 * Vector restrictions are essentially the same as
 * assignment to a string, however there are no issues
 * with the source element sizes not fitting into the destination
 * array elements.
 * \param rhs   - vector of data to store into the array channel.
 * \return std::vector<int>&
 * \retval Reference to rhs.
 */
const vector<int>&
CChannel::operator=(const vector<int>& rhs)
{
	
	if (m_fConnected) {
		CCriticalRegion Monitor(m_Monitor);
		// Figure out my counts, and storage requirements.
		
		int count = rhs.size();
		if (count > ca_element_count(m_nChannel)) {
			count = ca_element_count(m_nChannel);
		}
#ifdef _WINDOWS
        size_t bytesNeeded      = dbr_buffersize(dbr_long_t, count);
#else
		size_t bytesNeeded      = dbr_size_n(DBR_LONG, count);
#endif
		dbr_long_t* pStorage     = (dbr_long_t*)malloc(bytesNeeded);
		
		if (pStorage) {
			dbr_long_t* pDest = pStorage;
			for (int i=0; i < count; i++) {
				*pDest++ = rhs[i];
			}
			// Set the channel; flush the buffers:
			
			ca_array_put(DBR_LONG, count, m_nChannel, pStorage);
			ca_flush_io();
			free(pStorage);
			
		}
		
	}
	
	return rhs;
}
/*!
 * Assign a vector of doubles t to an array channel.
 * \param rhs - vector of data to store.
 * \return std::vector<double>
 * \retval Reference to rhs.
 */
const vector<double>&
CChannel::operator=(const vector<double>& rhs)
{
	if (m_fConnected) {
		CCriticalRegion Monitor(m_Monitor);
		// Figure out my counts, and storage requirements.
		
		int count = rhs.size();
		if (count > ca_element_count(m_nChannel)) {
			count = ca_element_count(m_nChannel);
		}
#ifdef _WINDOWS
        size_t bytesNeeded      = dbr_buffersize(dbr_double_t, count);
#else
		size_t bytesNeeded      = dbr_size_n(DBR_DOUBLE, count);
#endif
		dbr_double_t* pStorage     = (dbr_double_t*)malloc(bytesNeeded);
		
		if (pStorage) {
			dbr_double_t* pDest = pStorage;
			for (int i=0; i < count; i++) {
				*pDest++ = rhs[i];
			}
			// Set the channel; flush the buffers:
			
			ca_array_put(DBR_DOUBLE, count, m_nChannel, pStorage);
			ca_flush_io();
			free(pStorage);
			
		}
		
	}
	
	return rhs;	
}

/**
 * This function is a class level function that processes EPICS events
 * for some fixed number of seconds.
 */
void
CChannel::doEvents(float seconds)
{

  ca_pend_event(seconds);
}


/**
 * This function is a class level function that gets the
 * channel state change events.  The channel has associated with it
 * a pointer to a CChannel object.  We use this to establish
 * object context and manipulate the actual connection status.
 *
 */
void
CChannel::StateHandler(connection_handler_args args)
{
  chid            id = args.chid;
  long            op = args.op;
  CChannel* pChannel = (CChannel*)ca_puser(id);
  CCriticalRegion Monitor(pChannel->m_Monitor);   // Ensure the world is ours...

  // What we do now depends on the what's happened:

  if (op == CA_OP_CONN_UP) {	// Just connected...
    pChannel->m_fConnected = true;
    if (!pChannel->m_pConverter) {
      pChannel->m_pConverter = CConversionFactory::Converter(ca_field_type(id));
    }
    if(!pChannel->m_fUpdateHandlerEstablished) {
    }
    if(!pChannel->m_fUpdateHandlerEstablished) {

      ca_create_subscription(pChannel->m_pConverter->requestType(),
			    0, id, DBE_VALUE, 
			    UpdateHandler, (void*)pChannel,
			    &(pChannel->m_updateEventId));

      pChannel->m_fUpdateHandlerEstablished = true;
    }
  }
  else if (op == CA_OP_CONN_DOWN) { // just disconnected
    pChannel->m_fConnected = false;
    ca_clear_subscription(pChannel->m_updateEventId);
    pChannel->m_fUpdateHandlerEstablished = false;
    delete pChannel->m_pConverter;
    pChannel->m_pConverter = 0;
  }
  else {			// none of the above.
    // TODO: Figure out appropriate error handling here.
  }
}
/**
 * The actual update handler.
 * @param args -. Event handler structure that contains everything we need to
 *                process the event.
 *
 * The caller ensures that the status is ECA_NORMAL for us
 */
void
CChannel::Update(event_handler_args args)
{

  CCriticalRegion lock(m_Monitor);	// This entire functions is critical.

  // There should be a converter but double check just in case:

  if(m_pConverter) {
    
    // Update the last update time.

    time_t now = time(NULL);
    m_LastUpdateTime = now;

    // Update the simple string value.
    
    m_sValue  = (*m_pConverter)(args);
    
    // Update the cached vector value

    m_VectorValue = m_pConverter->getVector(args);
  }
  // Fire any slot function...with the mutex held to ensure stability:

  if (m_pHandler) {
    (*m_pHandler)(this, m_pHandlerData);
  }
  
}  

/**
 * Called in response to a channel update event.
 * - args.usr  - points to the object associated with the event.
 * - args.dbr  - is a const char* for the value....
 *               but only if:
 * - args.status - ECA_NORMAL for dbr to be valid else something else.
 *
 */
void
CChannel::UpdateHandler(event_handler_args args)
{

  if(args.status == ECA_NORMAL) {
    CChannel* pChannel = (CChannel*)args.usr;
    CCriticalRegion Monitor(pChannel->m_Monitor);   // Ensure the world is ours.


    if(pChannel->m_pConverter) {
      time_t    now      = time(NULL); // Last update time.
      pChannel->m_LastUpdateTime = now;
      pChannel->m_sValue = (*(pChannel->m_pConverter))(args);

      // If necessary, invoke the user's update handler.1

      if(pChannel->m_pHandler) {
	(pChannel->m_pHandler)(pChannel, pChannel->m_pHandlerData);
      }
    }
  }
  else {
    // TODO:  Figure out appropriate error action if any.
  }
}



/*!
 *  Connection factory converter instantiator.
 *   \param type - the underlying epics data type of the channel.
 */
CConverter*
CConversionFactory::Converter(short type) {
  switch (type) {
  case DBF_STRING:
    return new CStringConverter;
    break;
  case DBF_CHAR:
  case DBF_SHORT:
  case DBF_LONG:
    return new CIntegerConverter;
    break;
  case DBF_ENUM:
    return new CEnumConverter;
    break;
  case DBF_FLOAT:
  case DBF_DOUBLE:
    return new CFloatConverter;
    break;
    // Default is a string as well:
  default:
    return new CStringConverter;
  }
}



/////////////////////////////////////////////////////////////
/*!
   Return the request type appropriate to a string converter
   (DBF_STRING)
*/
short
CStringConverter::requestType()
{
  return DBR_STRING;
}
/*!
   Convert a string data type to a string... basically
   just casting/assignment
*/
string
CStringConverter::operator()(event_handler_args args)
{
  return convert(args.dbr);
}
/*
 * Convert an alement to a string.  This is common code
 * between getVector and operator()
 */
string
CStringConverter::convert(const void* element)
{
	return string((char*)element);
}

/*
 *  Return the alowed values for the channel in this
 * case it will be the string "string"
 */
std::vector<string> 
CStringConverter::allowedValues() const
{
	vector<string> result;
	result.push_back(string("string"));
	return result;
}
/*
 * Return a converted vector of values.
 */
vector<string>
CStringConverter::getVector(event_handler_args args)
{

	vector<string> result;
	long           nStrings = args.count;
	const dbr_string_t*    pStrings = reinterpret_cast<const dbr_string_t*>(args.dbr);
	
	for (long i= 0; i < nStrings; i++) {
	  result.push_back(convert(*pStrings));
	  pStrings++;
	}
	return result;

}
////////////////////////////////////////////////////////////
/*!
   Return the request type appropriate to an integer converter
   (DBF_LONG)
*/
short
CIntegerConverter::requestType()
{
  return DBR_LONG;
}
/*!
   Return the stringified version of an integer value.
*/
string
CIntegerConverter::operator()(event_handler_args args)
{
  return convert(args.dbr);
}
/*!
 *  Convert a single value.  This is code commonly used between
 * the operator() and getVector to convert a single element to
 * a string given a pointer to that element.
 */
string
CIntegerConverter::convert(const void* element)
{
	  char buffer[100];
	  sprintf(buffer, "%ld", *((long*)element));
	  return string(buffer);

}
/*!
 *  Return the allowed values for this type.
 * In this case it's the string "int"
 */
std::vector<string> 
CIntegerConverter::allowedValues() const
{
	vector<string> result;
	result.push_back(string("int"));
	return result;
}
/*! 
 * Get and convert a vector of values.
 */
vector<string>
CIntegerConverter::getVector(event_handler_args args)
{

  long          count   = args.count;
  const dbr_long_t*   pValues = reinterpret_cast<const dbr_long_t*>(args.dbr);
  vector<string> result;

  for (long i =0; i < count; i++) {
    result.push_back(convert(pValues));
    pValues++;
  }
  return result;

}
////////////////////////////////////////////////////////////


/*!
    Return the request type appropriate to a floating point value
   conversion (DBF_DOUBLE)
*/
short
CFloatConverter::requestType()
{
  return DBR_DOUBLE;
}
/*!
   Return the stringified version of a double channel
*/
string
CFloatConverter::operator()(event_handler_args args)
{
  return convert(args.dbr);

}
/*
 *   Utility function used by both the oeprator() and
 *   getVector members to convert a single item.
 */
string
CFloatConverter::convert(const void* element)
{
	const dbr_double_t* value = static_cast<const dbr_double_t*>(element);
	char buffer[100];
	sprintf(buffer, "%.7g", *value);
	return string(buffer);
}
/*!
 *    Return the allowed values for a double
 *    this will be the string float.
 */
std::vector<string> 
CFloatConverter::allowedValues() const
{
	vector<string> result;
	result.push_back(string("float"));
	return result;
}
/*!
 *   Return a converted vector.
 */
vector<string>
CFloatConverter::getVector(event_handler_args args)
{
	vector<string>       result;
	long                 nValues = args.count;
	const dbr_double_t*  pValues = reinterpret_cast<const dbr_double_t*>(args.dbr);

	for (long i = 0; i <nValues; i++) {
	  result.push_back(convert(pValues));
	  pValues++;
	} 
	return result;

}
///////////////////////////////////////////////////////////////
/*!
 *   Return the request type for an enumerated value
 */
short
CEnumConverter::requestType()
{
	return DBR_CTRL_ENUM;               // Get the whole enum record.
}
/*!
 *   Convert the event into a value.
 *   If we don't have a list of strings yet, these are extracted
 *   from the event. 
 *   Conversion works as follows.  If 'value' is in the range of
 *   the string vector the string is looked up and returned.
 *   If, however, the string is empty or the value is outside of
 *   the range of array indices, the value is turned into an integer
 */
string
CEnumConverter::operator()(event_handler_args args)
{
	return convert(args.dbr);
}
/*!
 *  Convert a channel data element to its string representation.
 *  See operator() for more information.  This code used to live
 *  in operator() but as it's common between that and getVector
 *  it has been factored out insted.
 */
string
CEnumConverter::convert(const void* element) 
{
    const struct dbr_gr_enum* pValue = static_cast<const struct dbr_gr_enum*>(element);
	
	// If needed build up the allowed values string.
	
	if (m_allowedValues.size() == 0) {
		for (int i=0; i < pValue->no_str; i++) {
			string  value = pValue->strs[i];
			m_allowedValues.push_back(value);
		}
	}
	// Convert the int value to a string:
	
	dbr_enum_t value = pValue->value;
	if (value < m_allowedValues.size()) {
		string result = m_allowedValues[value];
		if (result != "") {
			return result;
		}
	}
	// Either there's a blank string or the index is bad:
	
	char cszResult[1000];
	sprintf(cszResult, "%i", value);
	return string(cszResult);
}
/*!
 *   Return the set of allowed values
 *   For us, this is just the m_allowedValue vector
 *   (or rather a copy of it).
 */
vector<string>
CEnumConverter::allowedValues() const
{
	return m_allowedValues;
}
/*!
 * Return a vector of converted values
 */
vector<string>
CEnumConverter::getVector(event_handler_args args)
{
	vector<string> result;
	long           nItems = args.count;
	const dbr_gr_enum*   pItems = reinterpret_cast<const dbr_gr_enum*>(args.dbr);

	for (long i = 0; i < nItems; i++) {
	  result.push_back(convert(pItems));
	  pItems++;
	}
	return result;

}
