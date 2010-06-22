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


// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CCHANNELVISITOR_H  //Required for current class
#define __CCHANNELVISITOR_H

// Forward definitions:

class CChannel;

/*!
  ABC for a channel visitor.
Channel visitors are the parameters to
CChannelList::foreach.  A channel visitor is
a function object that implements an
operator()(CChannel*).

*/
class CChannelVisitor      
{
  // Class operations:
  
  // Need to virtualize the destuctor.

public:
  virtual ~CChannelVisitor() {}
  
public:
  
  virtual void operator() (CChannel* pChannel)   = 0 ; //!< Per channel action.
  
};

#endif
