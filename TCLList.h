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

//  CTCLList.h:
//
//    This file defines the CTCLList class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////
/*!
    Change Log:
    $Log$
    Revision 1.2  2007/04/25 21:23:18  fox
    Add stuff to build windows version

    Revision 1.1  2007/03/21 15:24:47  fox
    Initial installation of the epicstcl package into the stripper foil
    CVS repository

    Revision 8.3  2006/03/16 16:57:50  ron-fox
    Import tclPlus (TCL DIr) from SpecTcl-3.1 where it tries to get rid
    of most deprecated calls and adds docbook -> manpage documentation for the
    entire library

    Revision 5.3  2006/03/10 14:21:53  ron-fox
    Migrate TCL Interfaces to object form and remove direct
    references to interp->result

    Revision 5.2  2005/06/03 15:19:30  ron-fox
    Part of breaking off /merging branch to start 3.1 development

    Revision 5.1.2.1  2004/12/21 17:51:27  ron-fox
    Port to gcc 3.x compilers.

    Revision 5.1  2004/11/29 16:56:14  ron-fox
    Begin port to 3.x compilers calling this 3.0

    Revision 4.2.4.1  2004/10/20 19:53:02  ron-fox
    Fix a defect discovered in TCLList::operator= attempt to allow variables
    as axis specs (failed).

    Revision 4.2  2003/03/25 12:01:28  ron-fox
    Added Change log comment generated from the CVS $Log$
    Added Change log comment generated from the CVS Revision 1.2  2007/04/25 21:23:18  fox
    Added Change log comment generated from the CVS Add stuff to build windows version
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 1.1  2007/03/21 15:24:47  fox
    Added Change log comment generated from the CVS Initial installation of the epicstcl package into the stripper foil
    Added Change log comment generated from the CVS CVS repository
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 8.3  2006/03/16 16:57:50  ron-fox
    Added Change log comment generated from the CVS Import tclPlus (TCL DIr) from SpecTcl-3.1 where it tries to get rid
    Added Change log comment generated from the CVS of most deprecated calls and adds docbook -> manpage documentation for the
    Added Change log comment generated from the CVS entire library
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 5.3  2006/03/10 14:21:53  ron-fox
    Added Change log comment generated from the CVS Migrate TCL Interfaces to object form and remove direct
    Added Change log comment generated from the CVS references to interp->result
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 5.2  2005/06/03 15:19:30  ron-fox
    Added Change log comment generated from the CVS Part of breaking off /merging branch to start 3.1 development
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 5.1.2.1  2004/12/21 17:51:27  ron-fox
    Added Change log comment generated from the CVS Port to gcc 3.x compilers.
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 5.1  2004/11/29 16:56:14  ron-fox
    Added Change log comment generated from the CVS Begin port to 3.x compilers calling this 3.0
    Added Change log comment generated from the CVS
    Added Change log comment generated from the CVS Revision 4.2.4.1  2004/10/20 19:53:02  ron-fox
    Added Change log comment generated from the CVS Fix a defect discovered in TCLList::operator= attempt to allow variables
    Added Change log comment generated from the CVS as axis specs (failed).
    Added Change log comment generated from the CVS tag.
*/
#ifndef __TCLLIST_H  //Required for current class
#define __TCLLIST_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
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

typedef std::vector<std::string> StringArray;
typedef StringArray::iterator StringArrayIterator;

class CTCLList  : public CTCLInterpreterObject        
{
  char* m_pList;  // Pointer to list storage, dynamically allocated
  
public:
			//Default constructor

  CTCLList (CTCLInterpreter* pInterp) :
    CTCLInterpreterObject(pInterp),
    m_pList(0) 
  { }
  ~CTCLList() { delete []m_pList; };       //Destructor

			//Constructors with arguments

  CTCLList (CTCLInterpreter* pInterp, const  char* am_pList  );       
  CTCLList (CTCLInterpreter* pInterp, const std::string& rList);
	
			//Copy constructor

  CTCLList (const CTCLList& aCTCLList ) {
    DoAssign(aCTCLList);
  }


			//Operator= Assignment Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CTCLList& operator= (const CTCLList& aCTCLList) {
    if(this != &aCTCLList) {
      //      CTCLList::operator=(aCTCLList);
      DoAssign(aCTCLList);
    }
    return *this;
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CTCLList& aCTCLList);
  int operator!= (const CTCLList& aCTCLList) {
    return !(operator==(aCTCLList));
  }

  // Selectors:

public:
  const char* getList() const
  {
    return m_pList;
  }
  // Mutators:

protected:                
  void setList (const char* am_pList);
  
  // Operations:
  // 
public:
  // Operations which reflect the Tcl_xxx operations on lists.

  int Split (StringArray& rElements)  ;
  int Split (int& argc, char*** argv);

  const char* Merge (const StringArray& rElements)  ;
  const char* Merge(int argc, char** argv);
  //
  // protected utilities:
  //
protected:
  void DoAssign(const CTCLList& rRhs);

};

#endif
