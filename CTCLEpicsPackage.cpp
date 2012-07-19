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

static struct ca_client_context* pEpicsContext;

static const char* version= "2.1";

static const int caPollInterval(16); // ms per epics poll.


// Ensures that ca_pend_event is called often enough
// for epics to stay happy.
//

static void pollEpics(ClientData ignored) {

  ca_attach_context(pEpicsContext);
  while (1) {
    CChannel::doEvents(0.0);
    //  Tcl_CreateTimerHandler(caPollInterval, pollEpics, (ClientData)NULL);
  }
}




extern "C" {
#ifdef WIN32
__declspec(dllexport)
#endif
  int Epics_Init(Tcl_Interp* pInterp)
  {

    Tcl_ThreadId pollId;
#ifdef USE_TCL_STUBS
    Tcl_InitStubs(pInterp, "8.3", 0);
#endif

    Tcl_PkgProvide(pInterp, "epics", version);

    CTCLInterpreter* pI = new CTCLInterpreter(pInterp);	
    new CTCLEpicsCommand(*pI);
    // pollEpics((ClientData)NULL);
    ca_context_create(ca_enable_preemptive_callback);
    pEpicsContext = ca_current_context();
    Tcl_CreateThread(&pollId, pollEpics, NULL, TCL_THREAD_STACK_DEFAULT, TCL_THREAD_NOFLAGS);


    return TCL_OK;		// Don't start deadlock killer.

  }
  void* gpTCLApplication(0);

}

