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


#include "CTCLEpicsCommand.h"
#include "CTCLChannelCommand.h"
#include <CChannel.h>

#include <TCLInterpreter.h>
#include <cadef.h>
#include <map>
#include <string>
#include <tcl.h>
#include <tk.h>

using namespace std;



static char* version= "2.0";

static const int caPollInterval(16); // ms per epics poll.


// Ensures that ca_pend_event is called often enough
// for epics to stay happy.
//

static void pollEpics(ClientData ignored) {

  CChannel::doEvents(0.01);
  Tcl_CreateTimerHandler(caPollInterval, pollEpics, (ClientData)NULL);

}




extern "C" {
#ifdef WIN32
__declspec(dllexport)
#endif
  int Epics_Init(Tcl_Interp* pInterp)
  {

#ifdef USE_TCL_STUBS
    Tcl_InitStubs(pInterp, "8.3", 0);
#endif

    Tcl_PkgProvide(pInterp, "epics", version);

    CTCLInterpreter* pI = new CTCLInterpreter(pInterp);	
    new CTCLEpicsCommand(*pI);
    pollEpics((ClientData)NULL);



    return TCL_OK;		// Don't start deadlock killer.

  }
  void* gpTCLApplication(0);

}

