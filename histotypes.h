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
/*
** histotypes.h:
**    This file defines data types which are used by the 
**    histogramming package.
**
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   mailto: fox@nscl.msu.edu
**
** (c) Copyright 1999 NSCL, All rights reserved.
*/

#ifndef __HISTOTYPES_H
#define __HISTOTYPES_H

// Requires include for <config.h>

#ifndef __IOSTREAM_DAQH
#include <iostream>
#endif


#ifndef __CPP_STRING_H
#include <string.h>
#define __CPP_STRING_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif


typedef int Int_t;
typedef unsigned int UInt_t;

typedef short Short_t;
typedef unsigned short UShort_t;

typedef long Long_t;
typedef unsigned long ULong_t;

typedef float Float_t;
typedef double DFloat_t;



typedef char Char_t;
typedef unsigned char UChar_t;

typedef char (*Textsz_t);             // Null terminated string.
typedef UInt_t Size_t;

typedef void (*Address_t);           // Typical pointer.

static const Address_t kpNULL = (Address_t)0;  // Null pointer.

typedef enum {                    // State of a file.
	           kfsOpen,
		   kfsClosed
} FileState_t;

typedef enum {
  kacRead   = 1,		// Read access
  kacWrite  = 2,		// Write access
  kacCreate = 4,		// Create if needed.
  kacAppend = 8			// Append before writes.

} Access_t;

#define kACTIONSIZE 1024 // Size of action message.


typedef UChar_t Bool_t;
static const Bool_t kfTRUE = 0xff; // TRUE boolean.
static const Bool_t kfFALSE= 0;	  // FALSE boolean. 

typedef struct {
  UShort_t month,day, year;	//  date.
  UShort_t hours, min, sec;	// time in day.
} Time_t, *pTime_t;

typedef enum _Datatype_t {
  keByte,			// Data type is byte.
  keWord,			// Data type is word.
  keLong,			// Data type is a long.
  keFloat,			// Data type is single precision float.
  keDouble,			// Data type is double precision float.
  keUnknown_dt
} DataType_t;

inline std::ostream&
operator<<(std::ostream& out, DataType_t t)
{
  switch(t) {
  case keByte:
    out << "byte ";
    break;
  case keWord:
    out << "word ";
    break;
  case keLong:
    out << "long ";
    break;
  case keFloat:
    out << "float ";
    break;
  case keDouble:
    out << "double ";
    break;
  case keUnknown_dt:
  default:
    out << "???";
    break;
  }
  return out;
}
#ifndef WIN32                // KLUDGE KLUDGE KLUDGE
inline std::istream&
operator>>(std::istream& in, DataType_t& t)
{
  std::string value;
  t = keUnknown_dt;
  in >> value;
  if(value == std::string("byte")) t =  keByte;
  if(value == std::string("word")) t = keWord;
  if(value == std::string("long")) t = keLong;
  if(value == std::string("float"))t =  keFloat;
  if(value == std::string("double"))t = keDouble;
  return in;
}
#endif


typedef enum {			// Types of display program gates.
   kgCut1d,			// 1-d cut gate (upper lower limit)
   kgContour2d,		// 2-d closed countour
   kgBand2d,			// 2-d open band
   kgUnSpecified		// Gate type not yet known.
} GateType_t;

typedef struct {		// A set of coordinates.
  Int_t nX;
  Int_t nY;
} Point_t;

typedef enum {			// types of spectra in prompter.
  keAny,			// Any type is legit.
  ke1d,				// 1-d only.
  ke2d,				// 2-d only.
  keCompatible			// Only those compatible with selected.
} DialogSpectrumType_t;

typedef enum _SpectrumType_t {
  ke1D,
  ke2D,
  keBitmask,
  keSummary,
  keUnknown
} SpectrumType_t;


// I/O for spectrum types.

inline std::ostream& 
operator<<(std::ostream& out, SpectrumType_t t)
{
  switch(t) {
  case ke1D:
    out << '1';
    break;
  case ke2D:
    out << '2';
    break;
  case keBitmask:
    out << 'b';
    break;
  case keSummary:
    out << 's';
    break;
  case keUnknown:
  default:
    out << '?';
    break;

  }
  return out;
}

inline std::istream& 
operator>>(std::istream& in, SpectrumType_t& t)
{
  char c;
  in >> c;
  switch(c) {
  case '1':
    t = ke1D;
    break;
  case '2':
    t = ke2D;
    break;
  case 'b':
    t = keBitmask;
    break;
  case 's':
    t = keSummary;
    break;
  case '?':
  default:
    t = keUnknown;
    break;
  }
  return in;
}

static const UInt_t kn1K                  = 1024;
static const UInt_t kn1M                  = kn1K*kn1K;

// Default buffer size:

static const UInt_t knDefaultBufferSize   = kn1K*8;

// Default histogram storage size

static const UInt_t knDefaultSpectrumSize = 16*kn1M;

#endif


