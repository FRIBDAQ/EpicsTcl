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
#  Provide a widget that displays an epics channel along with its
#  .EGU field.  In order to support all the options and
#   methods of a label this is now a single label with
#   traces doing the update of the text.
#
package provide epicsLabelWithUnits 1.0


package require Tk
package require snit
package require epics
package require bindDown

namespace eval controlwidget {
    namespace export epicsLabelwithUnits
}

snit::widget ::controlwidget::epicsLabelWithUnits {
    option -channel {}
    delegate option * to label
    delegate method * to label

    constructor args {

	# first set up the widgets so we have something to configure

	install label using label $win.label

	pack $win.label -fill both -expand 1

	bindDown $win $win
	
	# Configure the widgets and connect them to epics.

	$self configurelist $args
	set channel $options(-channel)
	if {$channel eq ""} {
	    error "epicsLavelWithUnits must have a -channel on construction."
	}

	epicschannel $channel
	epicschannel ${channel}.EGU
	$channel link       ::controlwidget::$channel
	${channel}.EGU link ::controlwidget::${channel}.EGU

	$win.label config \
	    -text "[set ::controlwidget::$channel] [set ::controlwidget::${channel}.EGU]"

	trace add variable ::controlwidget::$channel       write \
	    [mymethod updateLabel]
	trace add variable ::controlwidget::${channel}.EGU write \
	    [mymethod updateLabel]

    }
    #  The destructor will
    #   - unset the traces.
    #   - unlink the variables taking advantage of ref counts to know
    #     this is safe.
    #   - destroy the channel again secure in the knowledge that
    #     refcounts will make this safe.
    #
    destructor {
	set channel $options(-channel)
	if {$channel eq ""} {
	    return 
	}

	trace remove variable ::controlwidget::$channel       write \
	    [mymethod updateLabel]
	trace remove variable ::controlwidget::${channel}.EGU write \
	    [mymethod updateLabel]

	$channel unlink ::controlwidget::$channel
	${channel}.EGU unlink ::controlwidget::${channel}.EGU

	${channel}     delete
	${channel}.EGU delete
	
    }
    #  Trace function. We must build up the label text from the
    #  values of the variables linked to the channel and the units:
    #
    method updateLabel {name1 name2 op} {
	set channel $options(-channel)
	if {$channel eq ""} {
	    return
	}

	$win.label config \
	    -text "[set ::controlwidget::$channel] [set ::controlwidget::${channel}.EGU]"

    }

}
