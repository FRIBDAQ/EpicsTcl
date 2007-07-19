#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2005.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#             Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

package provide epicsPullDown 1.0
package require Tk
package require epics
package require snit


# epicsPulldown is an epics aware pull down menu.
# The Menu is implemented as a tk menu button.
# The widget is a widget adaptor around the menu button
# so that as much as possible menu button options etc.
# are supported.
#
# OPTIONS:
#    -channel    - The epics channel controlled by the menu.
#                  must be supplied when the widget is created
#                  and is not dynamic.
#    -items      - A list of items to appear in the menu.  These
#                  items are one or two element lists.
#                    item         -  The menu item label, and value to write when
#                                   selected.
#                    {item value} - the label, value is written when selected.
#                    -            - A separator is inserted.
#                  Can be dynamically modified.
#   -tearoff     - The menu is created as a tearoff menu, that is there's a dashed line
#                  that can be clicked to tear the menu off into a top level window.
#                  false by default.
#
#    All menubutton options except:
#        -menu
# The menu button displays the current value of the channel as follows:
#   - If the value corresponds to one of the menu items, the menu label
#     is displayed (e.g. if the value is 5 and there's a menu item
#     ("Foil 6" 5}  The text "Foil 6" is displayed.
#   - If the value does not correspond to one of the menu items,
#     the raw chanel value is displayed.
#
namespace eval controlwidget { }

snit::widgetadaptor controlwidget::epicsPullDown {
	option -channel  {}
	option -items    {}

	
	variable channelValue
	variable radioButtonValue

        delegate option -tearoff to menu
	delegate option * to hull
	delegate method * to hull
	
	constructor args {
	    installhull using menubutton
	    install menu as menu $win.menu
	    
	    $self configurelist $args

	    $win configure -menu $win.menu
	    
	    
	    if {$options(-channel) eq ""} {
		error "controlwidget::epicsPullDown must specify a -channel when created."
	    }
	    epicschannel $options(-channel)
	    
	    trace  add variable ${selfns}::channelValue write [mymethod updateButton]
	    $options(-channel) link ${selfns}::channelValue
	    
	    # May not need this given the order of the code above....
	    
	    after 100 [mymethod updateButton \
			   ${selfns}::channelValue \
			   "" write];                    # get initial value right.
	    
	}

	#-------------------------------------------------------------------------
	#
	#  process the -items configuration  See the top level comments for a 
	#  description of this value.  We will emptythe menu and rebuild it:
	#
	onconfigure -items itemlist {
		$win.menu delete 0 end
		
		foreach item $itemlist {
			if {$item eq "-"} {
				#
				# Add a separator:
				#
				$win.menu add separator
				
			} elseif {[llength $item] == 1} {
				#
				#  Label and value are the same:
				#
			    set label [lindex $item 0]
				$win.menu add radiobutton         	\
				                                -label    $label   		\
								-command  [mymethod changeValue $label] \
								-variable ${selfns}::radioButtonValue \
								-value    $label
				
			} elseif {[llength $item] == 2} {
				#
				#  Same as above, but the item label and item value
				#  need not be the same:
				#
				set label [lindex $item 0]
				set value [lindex $item 1]
				$win.menu add radiobutton             					\
								-label    $label			 			\
								-command  [mymethod changeValue $value]	\
								-variable ${selfns}::radioButtonValue	\
								-value    $value
			} else {
				error "-items item: $item has invalid format"
			}
		}
		# Set options so that cget works transparently:
		
		set options(-items) $itemlist
	}
	#------------------------------------------------------------------------
	#
	#   This is called to update the label on the button.
	#   - The value of the channel is gotten.
	#   - If the value matches an item in the menu label lists, the
	#     label associated with that item is used.
	#   - If there is no match, the raw value is used instead.
	# Paramters (all ignored)
	#   name    - Name of the variable bound to the channel
	#   index   - index if the variable is an array element (it won't be).
	#   op      - Operation that fired this trace (always write).
	#   We are ensured at this point that the widgets have been made.
	#
	method updateButton {name index op} {
	    set label [getMatchingLabel $channelValue $options(-items)]

	    $win configure -text $label
	}
	#------------------------------------------------------------------
	# 
	# Called when a menu item was selected. The
	# control system process variable is set to a new value
	# depending on which menu item was selected.  We don't set the
	# label of the menu button because eventually, when the actual
	# value changes, the trace will fire updateButton and that will
	# get done.
	#
	# Parameters:
	#    newValue   - New value to attempt to set.
	# NOTE:
	#    If the channel is not yet connected, nothing will happen.
	#
	method changeValue newValue {
		catch {$options(-channel) set $newValue}
	}
	#------------------------------------------------------------------
	#  utility proc to match a label to a value.
	#  If there is no match, the value is returned:
	# Parameters:
	#   value   - some value from a channel.#
	#   itemList- list of menu items (see top level comments for format)
	#             by now the list is considered validated.
	# Returns:
	#   label of the matching item in the itemList or value if there
	#   is no match.`
	#
	proc getMatchingLabel {value itemList} {
		foreach item $itemList {
			if {$item ne "-"} {
				if {[llength $itemList] == 1} {
				    set label    $item
				    set matchTo  $item
				} else {
				    set label   [lindex $item 0]
				    set matchTo [lindex $item 1]
				}
				if {$value == $matchTo} {
					return $label
				}
			}
		}
		# No matching item:
			
		return $value
	}	
}
	

								