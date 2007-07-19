
#include "CChannel.h"
#include <iostream>

#ifndef _WINDOWS
#ifdef Linux
#define MUTEX_TYPE PTHREAD_MUTEX_RECURSIVE_NP
#else
#define MUTEX_TYPE PTHREAD_MUTEX_RECURSIVE
#endif
#endif


using namespace std;


// Local class to manage synchronization.
// Synchronization is handled by constructing this
// object on a mutex that is used to manage the monitor.
// destruction releases.  The safe way for example is:
//  {         // Start of critical region...
//   CCriticalRegion monitor(mutex);  // Mutex locked.
//    ...
//  }         // mutex unlocked.

#ifdef _WINDOWS
class CCriticalRegion {
private:
CRITICAL_SECTION*   m_pMutex;
public:
  CCriticalRegion(CRITICAL_SECTION& mutex) :
    m_pMutex(&mutex) {
    EnterCriticalSection(m_pMutex);
  }
  ~CCriticalRegion() {
    LeaveCriticalSection(m_pMutex);
  }
  // It is important to define the following as private to
  // ensure this mutex is not duplicated:
private:
  CCriticalRegion(const CCriticalRegion& rhs);
  CCriticalRegion& operator=(const CCriticalRegion& rhs);
};
#else
class CCriticalRegion {
private:
  pthread_mutex_t*   m_pMutex;
public:
  CCriticalRegion(pthread_mutex_t& mutex) :
    m_pMutex(&mutex) {
    pthread_mutex_lock(m_pMutex);
  }
  ~CCriticalRegion() {
    pthread_mutex_unlock(m_pMutex);
  }
  // It is important to define the following as private to
  // ensure this mutex is not duplicated:
private:
  CCriticalRegion(const CCriticalRegion& rhs);
  CCriticalRegion& operator=(const CCriticalRegion& rhs);
};
#endif

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
      ca_clear_event(m_updateEventId);	// Stop event update event dispatching.
    }
    else {
      // Need to establish a 'null' connection event as it may yet connect:
      // We'll establish something that just returns:

      ca_change_connection_event(m_nChannel, nullConnectionHandler);
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
    ca_search_and_connect(m_sName.c_str(), &m_nChannel, StateHandler, (void*)this);
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
    ca_put(DBF_STRING, m_nChannel, const_cast<char*>(value.c_str()));
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
    ca_put(DBF_INT, m_nChannel, &value);
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
    ca_put(DBF_DOUBLE, m_nChannel, &value);
    ca_flush_io();
  }
  return value;

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
    if(!pChannel->m_fUpdateHandlerEstablished) {
      pChannel->m_pConverter = CConversionFactory::Converter(ca_field_type(id));
      ca_add_event(pChannel->m_pConverter->requestType(), 
		   id, UpdateHandler, (void*)pChannel, &(pChannel->m_updateEventId));
      pChannel->m_fUpdateHandlerEstablished;
    }
  }
  else if (op == CA_OP_CONN_DOWN) { // just disconnected
    pChannel->m_fConnected = false;
    delete pChannel->m_pConverter;
    pChannel->m_pConverter = 0;
  }
  else {			// none of the above.
    // TODO: Figure out appropriate error handling here.
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

      // If necessary, invoke the user's update handler.

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
  return string((const char*)(args.dbr));
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
  char buffer[100];
  sprintf(buffer, "%ld", *((long*)(args.dbr)));
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
  char buffer[100];
  sprintf(buffer, "%g", (*(double*)(args.dbr)));
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
        const struct dbr_gr_enum* pValue = static_cast<const struct dbr_gr_enum*>(args.dbr);
	
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

