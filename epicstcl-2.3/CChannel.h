#ifndef __CCHANEL_H
#define __CHANNEL_H
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

#ifndef __EPICS_CADEF
#include <cadef.h>
#ifndef __EPICS_CADEF
#define __EPICS_CADEF
#endif
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

#ifndef __CRT_TIME
#include <time.h>
#ifndef __CRT_TIME
#define __CRT_TIME
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#ifndef _PTHREAD_H
#include <pthread.h>
#ifndef _PTHREAD_H
#define _PTHREAD_H
#endif
#endif
#endif

#include <typeinfo>

class CConverter;

#if defined(_WIN32) || defined(_WIN64)
typedef CRITICAL_SECTION   Mutex;
#else
typedef pthread_mutex_t    Mutex;
#endif

// Class to manage synchronization.
// Synchronization is handled by constructing this
// object on a mutex that is used to manage the monitor.
// destruction releases.  The safe way for example is:
//  {         // Start of critical region...
//   CCriticalRegion monitor(mutex);  // Mutex locked.
//    ...
//  }         // mutex unlocked.

#if defined(_WIN32) || defined(_WIN64)
class CCriticalRegion {
private:
  CRITICAL_SECTION*   m_pMutex;
public:
  CCriticalRegion(CRITICAL_SECTION& mutex) :
    m_pMutex(&mutex) {
    lock();
  }
  ~CCriticalRegion() {
    unlock();
  }

  void unlock() {
    LeaveCriticalSection(m_pMutex);
  }
  void lock() {
    EnterCriticalSection(m_pMutex);
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
    lock();
  }
  ~CCriticalRegion() {
    unlock();
  }
  // Allow manual manipulation of the lock:

  void unlock() {
    pthread_mutex_unlock(m_pMutex);
  }
  void lock() {
    pthread_mutex_lock(m_pMutex);
  }
  // It is important to define the following as private to
  // ensure this mutex is not duplicated:
private:
  CCriticalRegion(const CCriticalRegion& rhs);
  CCriticalRegion& operator=(const CCriticalRegion& rhs);
};
#endif

/**
 *   The CChannel class implements transparently updated EPICS channels for C++
 * programs.  Objects require a two-phase construction, as we won't be sure of the
 * timing of static construction vs. the EPICS startup.
 *  - The first phase intializes the class member data.
 *  - The second phase connects the channel to the EPICS updating mechanism.
 */

class CChannel 
{
public:
  typedef void (*Slot)(CChannel*, void*);
private:
  std::string   m_sName;
  bool          m_fConnected;
  chid          m_nChannel;
  bool          m_fUpdateHandlerEstablished;
  bool          m_fConnectionHandlerEstablished;
  std::string   m_sValue;
  std::vector<std::string> m_VectorValue;
  time_t        m_LastUpdateTime;
  CConverter*   m_pConverter;
  Slot          m_pHandler;
  void*         m_pHandlerData;
  evid          m_updateEventId;

  mutable Mutex m_Monitor;
public:
  CChannel(std::string name);
  virtual ~CChannel();

  // Forbidden operations

private:
  CChannel(const CChannel&);
  CChannel& operator=(const CChannel&);
  int operator==(const CChannel&) const;
  int operator!=(const CChannel&) const;
public:

  // Operations on the object:

  std::string getName()     const;
  bool        isConnected() const;

  virtual void Connect();
  time_t       getLastUpdate() const;
  std::string  getValue()      const;
  std::vector<std::string> getAllowedValues() const;

  size_t       size()  const;
  std::vector<std::string> getVector(size_t max = 0);
  
  void setSlot(Slot handler, void* data);

  // operations to set channel values
  // this is just an overloaded assign to the channel:
  // the return value is the thing assigned:

  std::string operator=(std::string value);
  int         operator=(int value);
  double      operator=(double value);
  

  const std::vector<std::string>& operator=(const std::vector<std::string>& rhs);
  const std::vector<int>&    operator=(const std::vector<int>& rhs);
  const std::vector<double>& operator=(const std::vector<double>& rhs);

  // Class level operations.

  static void doEvents(float seconds);

 private:
  void StateChange(connection_handler_args args);
  void Update(event_handler_args args);

protected:
  static void StateHandler(connection_handler_args args);
  static void UpdateHandler(event_handler_args     args);
};

//////////////////////////////////////////////////////////////////////////////
/*!
**   The classes below are used to convert between epics native values
**   and std::strings... this is required becuse epics does not always
**   choose a good conversion.
**
**   In addition a conversion factory is provided.
**   This is all much simpler than it looks.  There are basically
**   only the following conversion types that we entertain:
**    std::string : Ask the user to request DBF_STRING and convert that
**             as %s
**    uint:    Ask the user to request DBF_ULONG and convert that.
**             as %ul
**             NOTE: Due to some funkiness in the epics headers I don't
**             understand, these can'tbe made to work. at this time.
**    int      Ask the user to request DBF_LONG and convert that.
**             and convert that as %l
**    float:   Ask the user to request DBF_DOUBLE  and convert that.
**             as %12.9g
** 
**  The normal path usage of the stuff below is:
**    In the connection handler for the channel
**    get the underlying data type using ca_field_type
**    Pass that in to an invocation of 
**      CConversionFactory::Converter();
**    This dynamically allocates a converter, call the pointer to it
**     pConverter
**    Invoke pConverter->requestType()
**    And use the return value as the data type requested when
**    invoking ca_add_event on the channel.
**    In the UpdateHandler,
**       std::string value = (*pConverter)(args);
**    to get a nice std::string value returned for the channel.
**
*/

/*!
   Abstract CConverter base class.... this is pure abstract.
*/

class CConverter 
{
protected:
  Mutex *m_pMutex;
public:
  
  virtual short  requestType() = 0;
  virtual std::string operator()(event_handler_args args) = 0;
  virtual std::string convert(const void* element) = 0;
  virtual std::vector<std::string> allowedValues() const = 0;
  virtual std::vector<std::string> getVector(event_handler_args args) = 0;

};

class CStringConverter : public CConverter 
{
public:
  virtual short requestType();
  virtual std::string operator()(event_handler_args args);
  virtual std::string convert(const void* element);
  virtual std::vector<std::string> allowedValues() const;
  virtual std::vector<std::string> getVector(event_handler_args args);

};



class CIntegerConverter : public CConverter
{
public:
  virtual short requestType();
  virtual std::string operator()(event_handler_args args);
  virtual std::string convert(const void* element);
  virtual std::vector<std::string> allowedValues() const;
  virtual std::vector<std::string> getVector(event_handler_args args);
};

class CFloatConverter : public CConverter
{
public:
  virtual short requestType();
  virtual std::string operator()(event_handler_args args);
  virtual std::string convert(const void* element);
  virtual std::vector<std::string>  allowedValues() const;
  virtual std::vector<std::string> getVector(event_handler_args args);
};

class CEnumConverter : public CConverter
{
  std::vector<std::string> m_allowedValues;
 public:
  virtual short requestType();
  virtual std::string operator()(event_handler_args args);
  virtual std::string convert(const void* element);
  virtual std::vector<std::string> allowedValues() const;
  virtual std::vector<std::string> getVector(event_handler_args args);
	
};
/*!
   Provides the correct converter type for the
   particular epics data type.
   If no type exists, an exception is thrown.
*/
class CConversionFactory {
public:
  static CConverter* Converter(short type);
};
#endif
