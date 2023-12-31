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

#  This widget provides a Type and Go control.
#  The idea is that there is a label that can be tied
#  to a variable.  Below that is an entry field, 
#  and below that a button.   A click on the
#  button or a key <Enter> in the entry widget
#  Result in the invocation of a command script.
#
#
# Options:
#    All label options are delegated to the label widget.
#    -text    is delegated to the button widget.
#
#    -command  provides a script that will be invoked when
#              the entry has been accepted and validated
#              (see below).
#    -validate provides a script to validate the entry
#              This script is called prior to 
#              invoking the -command option.
#              If the result of the script is 1,
#              The entry is assumed to be valid, and the
#              -command script is invoked.  If the
#              result of the script is 0, the entry
#              is assumed to be invalid and the
#              -command script will not be invoked.
#   -orient    can take vertical or horizontal keywords.
#              and can only be specified at initial creation time.
#              If vertical, (default), the widget will look like this:
#               label
#               type-in
#               button
#              If horizontal, the widget will look like this:
#               label type-in button
#  -showlabel  Takes a boolean value, and is only processed at creation time.
#              If true (the default), the label widget is shown, otherwise
#              the label widget is not shown.  The label widget is always
#              created so that subclassed widgets can be simplified.
#
#
# Substitutions:
#   For both -command and -validate the following substitutions
#   are defined:
#    %W   - The widget id
#    %V   - The contents of the entry widget.
#
# Methods:
#   Get   - Gets the value of the entry widget.
#   Set   - Sets the value of the entry widget.
#   Invoke- Simulates a push of the buttons.
#
# Bindings:
#  <Enter>     - invokes the pushbutton, when detected in the entry widget.
#  <FocusOut>  - Restores the prior value of the text field.
#
#
package provide typeNGo  1.0
package require Tk
package require snit
package require bindDown

namespace eval controlwidget {
}

snit::widget controlwidget::typeNGo {

    option -orient  vertical
    option -showlabel true

    delegate option -text    to button
    delegate option -label   to label as -text
    delegate option  *        to label

    option   -command  [list]
    option   -validate [list]

    variable lastValue [list]

    constructor args {
	install label  as label $win.label
	install button as button $win.button -command [mymethod onClick]
	entry $win.entry

	$self configurelist $args

	# Default the button label to "Go"

	if {[$win.button cget -text] eq ""} {
	    $win.button configure -text "Go"
	}

	# Validate the -orient and -showlabel options:

	if {![string is boolean -strict $options(-showlabel)]} {
	    error "typeNGo -showlabel  must have a boolean value has $options(-showlabel)"
	}
	if {[lsearch -exact "horizontal vertical" $options(-orient)] == -1} {
	    error "typenGo -orient must have a value 'vertical' or 'horizontal' had: $options(-orient)"
	}

	if {$options(-orient) eq "vertical"} {
	    if {$options(-showlabel)} {
		grid $win.label  -sticky w
	    }
	    grid $win.entry  -sticky w
	    grid $win.button -sticky w
	} else {
	    if {$options(-showlabel)} {
		set labelgrid $win.label
	    } else {
		set labelgrid x
	    }
	    grid $labelgrid $win.entry $win.button
	}

	# bind <Enter> in the entry so that it calls invoke:

	bind $win.entry <Return> [mymethod Invoke]
	bind $win.entry <FocusOut> [mymethod restoreLast]

	bindDown $win $win

    }
    #
    #  Return the contents of the entry widget
    #
    method Get {} {
	return [$win.entry get]
    }
    #
    #  Set the value of the entry widget... does not commit.
    #
    method Set {value} {
	$win.entry delete 0 end
	$win.entry insert end $value

	set lastValue $value
    }
    #
    # Invokes the button.  This fires any command script
    # That has been bound to the button.
    #
    method Invoke {} {
	$win.button invoke;		# Fire the script.
    }
    #
    #   Private methods/procs

    # onClick is invoked by the button when it is clicked
    # or by the <Return> (which after all just invokes the button.
    # If there is a validation script, we dispatch it
    # and require a true result...
    # If there is no validation script or if the validation script
    # returned a true, then we dispatch the -command script if it's
    # defined.
    #  Note that the dispatch method does all substitutions.
    #
    method onClick {} {
	set ok 1
	if {$options(-validate) ne ""} {
	    set ok [$self dispatch -validate]
	}
	if {$ok} {
	    $self dispatch -command
	    set lastValue [$self Get]
	} else {
	    $self restoreLast;   # Restore prior valid? value.
	}
    }
    # dispatch - dispatches a script, doing appropriate substitutions.
    # The value of the script is returned if defined, else the return
    # value is not well defined.
    # 
    #
    method dispatch {opt} {
	set script $options($opt)
	if {$script ne ""} {
	    set value [$self Get]
	    set script [string map [list %W $win %V [list $value]] $script]
	    return [eval $script]
	}
    }
    #
    #  Restore the prior value of the widget.. This is done e.g. on
    # focus out events, validation failures etc.
    #
    method restoreLast {} {
	$self Set $lastValue
    }

}