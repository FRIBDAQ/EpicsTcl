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
#ifndef __ASSERTS_H
#define __ASSERTS_H

// Abbreviations for assertions in cppunit.

#define EQMSG(msg, a, b)   CPPUNIT_ASSERT_EQUAL_MESSAGE(msg,a,b)
#define EQ(a,b)            CPPUNIT_ASSERT_EQUAL(a,b)
#define ASSERT(expr)       CPPUNIT_ASSERT(expr)
#define FAIL(msg)          CPPUNIT_FAIL(msg)

// Macro to test for exceptions:

#define EXCEPTION(operation, type) \
   {                               \
     bool ok = false;              \
     try {                         \
         operation;                 \
     }                             \
     catch (type e) {              \
       ok = true;                  \
     }                             \
     ASSERT(ok);                   \
   }


#endif
