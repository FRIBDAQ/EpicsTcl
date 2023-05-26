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

#
#   This file implements a label widget that
#   tracks an epics channel.  It accepts all of the
#   options any other label widget accepts, except -textvariable and one additional one:
# OPTIONS:
#   -channel    The name of the epics channel to attach to the widget.
# NOTE:
# METHODS:
#   All widget commands supported by the entry widget.
#
#  Note that this widget maintains variables in the ::controlwidget
#  namespace to promote sharing of epics channel attached variables amonst
#  widgets that track the same thing.
#

package provide epicsLabel 1.0
package require Tk
package require snit
package require epics
package require bindDown

namespace eval controlwidget {
    namespace export epicsLabel
}

snit::widget  controlwidget::epicsLabel {
    delegate method * to label
    option   -channel {}
    delegate option * to label

    constructor args {
	install label using label $win.label 
	$self configurelist $args
	
	set channel $options(-channel)
	if {$channel eq ""} {
	    error "epicsLabel widgets need a -channel on construction"
	}
	epicschannel $channel

	$channel         link ::controlwidget::$channel
	$win.label configure -textvariable ::controlwidget::$channel


	pack $win.label  -side left -expand 1 -fill both

	bindDown $win $win

    }
}