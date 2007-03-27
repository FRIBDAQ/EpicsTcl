#
#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#     Author:
#             Ron Fox
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321
#

# Implements a simple meter based display for an arbitrary
# epics channel.  Note that for sensors with settable ranges
# that conform to the BCM interface see epicsBcmMeter e.g.
#
# The meter is a composite widget consisting of an epicsLabelWithUnits
# and a meter arranged as follows:
#
#     +---------------------------------+
#     |         channelname/label       |
#     |     [epicsLabelWithUnits]       |
#     |     +------------------+        |
#     |     |  meter widget    |        |
#     |     |                  |        |
#     |     +------------------+        |
#     |                                 |
#     +---------------------------------+
#

# Options:
#     All of the options of the meter widget are supported by
#     delegation to the meter widget...except variable.
#     -channel     - Epics channel to display on the widget.
#     -label       - If supplied, overrides the channel name as a label
#                    forr the widget.
#

package provide epicsMeter 1.0


namespace eval controlwidget {
    namespace export epicsMeter
}

package require snit
package require epics
package require meter
package require epicsLabelWithUnits


snit::widget ::controlwidget::epicsMeter {
    option   -channel 
    option   -label

    delegate option *  to meter except -variable

    constructor args {
	install meter as controlwidget::meter $win.meter
	$self configurelist $args

	#  Require a channel be supplied and figure out the
	#  label:

	set chan $options(-channel)
	if {$chan eq ""} {
	    error "Constructing an epicsMeter requires a -channel"
	}
	if {$options(-label) eq ""} {
	    set options(-label) $chan
	}
	#  Now we need to construct two more widgets, the overall label,
	#  the epicslabelwith units;  we're going to do a bit of cheating
	#  epicslabelwithunits will create the variable controlwidget::channel-name
	#  as well as connecting to the channel for us..we'll pass that on to 
	#  the meter.

	label $win.label -textvariable ${selfns}::options(-label)
	controlwidget::epicsLabelWithUnits $win.value -channel $options(-channel)

	$win.meter configure -variable ::controlwidget::$options(-channel)

	# Layout the widget:

	pack $win.label $win.value $win.meter  -side top

	#  set up bindings to propagate:

	bindDown $win $win

    }
}