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
package provide epicsButton 1.1

package require Tk
package require epics
package require snit

namespace eval controlwidget {
    namespace export onoffSingleButton
    namespace export onoffButtonPair
    namespace export epicsButton
}

#  Provides a single button that toggles between
#  on/off state:
#     +----------------------+
#     |   [ on|offlabel]     |
#     +----------------------+
#
# Options:
#   -onlabel   - Label for the button to turn the device on.
#   -offlabel  - Label for the button to turn the device off.
#   -oncolor   - Color of button when device is on.
#   -offcolor  - Color of button when device is off.
#   -oncommand - Script to execute when the device is turned on.
#   -offcommand- Script to execute when the device is turned off.
#
# Methods:
#   on     - Set state on.
#   off    - Set state off.
#   invoke - click the button.
#   toggle - Switch state.
#   get    - Get the current state.
#
snit::widget  ::controlwidget::onoffSingleButton {
    option -onlabel    On
    option -offlabel   Off
    option -oncolor    green
    option -offcolor   red
    option -oncommand  [list]
    option -offcommand [list]

    variable state     off

    constructor args {
	$self configurelist $args

	button $win.b                             \
	    -text $options(-onlabel)              \
	    -background $options(-offcolor)       \
	    -activebackground $options(-offcolor) \
	    -command [mymethod onClick]
	pack $win.b -expand 1 -fill both


    }

    # Process the button click (this is just toggle):

    method onClick {} {
	if {$state} {
	    set script $options(-offcommand)
	} else {
	    set script $options(-oncommand)
	}
	if {$script ne ""} {
	    eval $script
	}
    }
    method invoke {} {
	$self onClick
    }
    # Toggle the state:
    #
    method toggle {} {
	if {$state} {
	    $self off
	} else  {
	    $self on
	}
    }
    # Read the state:

    method get {} {
	return $state
    }
    # Turn ourselves on

    method on {} {
	$win.b configure                         \
	    -text $options(-offlabel)            \
	    -background $options(-oncolor)       \
	    -activebackground $options(-oncolor) 
	set state on
	    
    }
    # Turn ourself off:

    method off {} {
	$win.b configure                        \
	    -text $options(-onlabel)            \
	    -background $options(-offcolor)     \
	    -activebackground $options(-offcolor)
	set state off

    }
}

#  Provides a pair of buttons to turn something on or off.
#  Only one of the buttons will be enabled at any time.
#  the enabled button will show a status color
#
#  Options and  methods are identical to 
#    onOffSingleButton.
#
#  Layout
#   +------------------------------+
#   | [ on Label]   [off Lable ]   |
#   +------------------------------+
#
snit::widget  controlwidget::onoffButtonPair {
    option -onlabel    On
    option -offlabel   Off
    option -oncolor    green
    option -offcolor   red
    option -oncommand  [list]
    option -offcommand [list]

    variable state off

    constructor args {
	$self configurelist $args

	button $win.on                            \
	    -text $options(-onlabel)              \
	    -background $options(-offcolor)       \
	    -activebackground $options(-offcolor) \
	    -command [mymethod onOn]

	button $win.off                            \
	    -text $options(-offlabel)              \
	    -state disabled                        \
	    -command [mymethod onOff]
	
	grid $win.on $win.off -sticky nsew

    }
    # Turn the state on

    method on {} {
	set state on
	set color [[winfo toplevel $win] cget -background]

	$win.on configure                         \
	    -background $color                    \
	    -activebackground $color              \
	    -state disabled

	$win.off configure                         \
	    -background $options(-oncolor)         \
	    -activebackground $options(-oncolor)   \
	    -state normal
    }
    # Turn the state off:

    method off {} {
	set state off
	set color [[winfo toplevel $win] cget -background]
	$win.on configure                           \
	    -background $options(-offcolor)         \
	    -activebackground $options(-offcolor)   \
	    -state normal

	$win.off configure                           \
	    -background $color                    \
	    -activebackground $color              \
	    -state disabled
    }
    # Toggle the state:

    method toggle {} {
	if {$state} {
	    $self off
	} else {
	    $self on
	}
    }
    # Invoke the button:

    method invoke {} {
	if {$state} {
	    $self onOff
	} else {
	    $self offOn
	}
    }
    method onOff {} {
	set script $options(-offcommand)
	if {$script ne ""} {
	    eval $script
	}
    }
    method onOn {} {
	set script $options(-oncommand)
	if {$script ne ""} {
	    eval $script
	}
    }
}


#
#   Provides button control for a binary output.
#   This really provides a pair of control modalities:
#   - Single   A single button controls the binary output
#              flipping labels and colors as the state of the
#              binary output changes.
#   - Double   One button turns the bit on, the other off.
#              The state of the device is shown by the colors
#              of the buttons.

#
# Layout (single):
#     +----------------------+
#     |    <PV_name>         |
#     |   [ on|offlable]     |
#     +----------------------+
#
#   The on label is displayed when the button would
#   turn the output on, the off label when the button
#   would turn the output off.
#
# Layout double:
#     +------------------------+
#     | <PV_name>              |
#     | [on label] [off label] |
#     +------------------------+
#
#  The button that functions is lit with the on/off color
#
#
# Options:
#   -channel   - The channel that will be controled
#   -statechannel
#              - If defined the channel that will contain the
#                device state (may be different from the request.
#   -onvalue   - Thing to write to the device to turn it on.
#   -offvalue  - Thing to write to the device to turn it off
#   -onlabel   - The Label for the button that turns the module on.
#   -offlabel  - The Label for the buton that turns the module off
#   -oncolor   - Status button for the module currently on.
#   -offcolor  - Status button for the module currently off.
#   -modality  - Type of button to use (single/double).
#   -showlabel - Show or don't show the process variable button.
#
# Methods:
#   on          - Turn the device on.
#   off         - turn the device off.
#   get         - return the device state.
#
snit::widget controlwidget::epicsButton {
    option -channel
    option -statechannel
    option -onvalue       1
    option -offvalue      0
    option -onlabel       On
    option -offlabel      Off
    option -oncolor       Green
    option -offcolor      Red
    option -modality      single
    option -showlabel     true

    variable statechannel
    variable constructing true


    constructor args {
	$self configurelist $args

	set channel $options(-channel)
	if {$channel eq "" } {
	    error {epicsButton must have a -channel option when constructed}
	}
	if {($options(-modality) ne "single") && ($options(-modality) ne "double") } {
	    error {epicsbutton -modality must be single or double}
	}

	set statechannel $options(-statechannel) 
	if {$statechannel eq ""} {
	    set statechannel $channel
	}

	label $win.label -text $channel
	if {$options(-modality) eq "single"} {
	    controlwidget::onoffSingleButton $win.button   \
		-onlabel $options(-onlabel)                \
		-offlabel $options(-offlabel)              \
		-oncolor $options(-oncolor)                \
		-offcolor $options(-offcolor)              \
		-oncommand [mymethod onOn]                \
		-offcommand [mymethod onOff]

	} else {
	    controlwidget::onoffButtonPair $win.button   \
		-onlabel $options(-onlabel)                \
		-offlabel $options(-offlabel)              \
		-oncolor $options(-oncolor)                \
		-offcolor $options(-offcolor)              \
		-oncommand [mymethod onOn]                \
		-offcommand [mymethod onOff]
	}
	$self layout

	# set up access to epics:

	epicschannel $channel
	if {$statechannel ne $channel} {
	    epicschannel $statechannel
	}

	# link the state channel so we can trace it:

	$statechannel link ::controlwidget::$statechannel

	# Set a write trace on the state channel so that
	# we can update the button boxe's status too:

	trace add variable  ::controlwidget::$statechannel write [mymethod update]
	after 10 [list $self update ::controlwidget::$statechannel ""  write]
	set constructing false
	
    }
    # Handle the -showlabel option:

    onconfigure -showlabel value {
	set options(-showlabel) $value
	if {!$constructing} {
	    $self layout
	}
    }

    # Layout the widget geometry (forget it first if not constructing).

    method layout {} {
	if {!$constructing} {
	    pack forget $win.button
	    catch {pack forget $win.label}
	}
	
	if {$options(-showlabel)} {
	    pack $win.label -fill both -expand 1
	}
	pack $win.button -fill both -expand 1


    }

    # on/off/get/toggle/invoke get delegated to the button

    method on     {} { $win.button on }
    method off    {} { $win.button off }
    method get    {} { return [$win.button get] }
    method toggle {} { $win.button toggle }
    method invoke {} { $win.button invoke }

    #  handle the button clicks:

    method onOn {} {
	set channel $options(-channel) 
	$channel set $options(-onvalue)
    }
    method onOff {} {
	set channel $options(-channel)
	$channel set $options(-offvalue)
    }
    #  The status channel just changed:

    method update {name index op} {
	if {[catch {$statechannel get} value]} return
	if {$value} {
	    $self on
	} else {
	    $self off
	}
    }
}

#  Provides an epics command button.  This is just
#  a widget adaptor for a plain old epics button however:
#  -command is reserved to us
#  -channel connects the button to a channel
#  -value   describes what the button writes to the channel
# All other options are delegated to the button widget.
#
snit::widgetadaptor controlwidget::epicsCommandButton {
	option  -channel {}
	option  -value   {}
	
	delegate option * to hull except -command
	delegate method * to hull
	
	constructor {args} {
		installhull using button -command [mymethod sendValue] 
		$self configurelist $args
		
		if {$options(-channel) eq ""} {
			error "controlwidget::epicsCommandButton requires a -channel argument"
		}
		
		epicschannel $options(-channel)
	}
	#-------------------------------------------------------------------------
	#
	#  sendValue is invoked when the button is clicked. All we need to do
	#  is send $options(-value) to the channel.
	#
	method sendValue {} {
		$options(-channel) set $options(-value)
	}
}