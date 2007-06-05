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

#
#  epicsStripChar provides slight extensions to the
#  blt::stripchart widget to support plotting the time
#  evolution of an arbitrary set of epics channels.
#  The blt::stripchart widget is a very flexible widget.
#  rather than attempting to either encapsulate the entire
#  set of capabilities of this widget, we will exposte the
#  entire capability set of the widget but add methods
#  that are tailored towards displaying time traces of
#  epics channels on the plot.
#
# OPTIONS:
#    All blt::stripchart options are supported and delegated
#    to the base widget.
# METHODS:
#    All blt::stripchart methods are supported and delegated
#    to the base widget.  We add in addition to this:
#      addchannel name interval ?options?
#
#          the channel 'name' to the strip chart updating
#          every interval milliseconds. The options are
#          valid configuration options to the blt::stripchart
#          line object.  
#    
#          In fact what we will do with this command is create
#          a new BLT line displaying a pair of vectors that will
#          be updated with the time and value of the channel
#          at the periodicity requested.  The name of the line
#          will be returned so that you can do additional configurations.
#    
#          The line returned will also be a new Tcl command that will
#          allow you to further manipulate the channel line:
#          line clear
#              Clears the line data.
#          line clearfirst  n
#               Removes the first n data points from the line.
#          line keep        n
#               Configure the line to keep at most n elements (deleting the
#               oldest if necessary.  n = 0 means keep all elements.
#          line updateperiod n
#               Change the update period for the line to n milliseconds.
#               takes effect after the next scheduled update.
#          line get
#                Returns a list of time/value pairs that are the
#                data of the line. {t0 data0} {t1 data1} ...
#          line names
#                Returns the names of the BLT vectors that have the
#                times and channel data. {time data}
#     removechannel name
#         Destroys the line associated with channel name, and all
#         resources associated with it.
#
#
package provide epicsStripChart 1.0
package require Tk
package require snit
package require BLT
package require epics

#
#  First a helper type.
#  This object contains the time and parameter vectors of a channel
#  and updates them periodically (in other words it implements the
#  line and its methods.... see the main comment header for the
#  info for the methods.
#
#
snit::type controlwidget::channelHistory {
    option    -period
    option    -channel
    option    -timebase

    variable   lastPeriod
    variable   lastTime
    variable   maxKept
    variable   afterId  0

    variable   tZeroMs
    variable   startSeconds


    # Construct the line.

    constructor args {
	$self configurelist $args

	if {![string is integer -strict $options(-period)]} {
	    error "controlWidget::channelHistory must be constructed with an integer -period"
	}
	if {$options(-period)  <= 0} {
	    error "controlWidget::channelHistory must be constructed with a non negative period"
	}
	if {$options(-channel) eq ""} {
	    error "controlWidget::channelHistory must be constructed with a -channel option value"
	}
	if {![string is integer -strict $options(-timebase)]} {
	    error "controlWidget::channelHistory must be constructed with an integer -timebase"
	}

	# Init data:

	set afterId 0
	set maxKept 0
	set lastPeriod $options(-period)
	set lastTime  [expr 1.0*[clock seconds]]

	set tZeroMs   [clock clicks -milliseconds]
	set startSeconds [expr [clock seconds] - $options(-timebase)]


	#  Construct the two vectors...and the epics channel data.

	blt::vector create ${selfns}::time
	blt::vector create ${selfns}::data
	epicschannel $options(-channel)

	# Schedule updates:

	$self updateData
	
    }
    #   Destruction requires cancelation of the scheduled update,
    #   and destruction of the vectors and channels:
    #   Note that the reference counted nature of epicschannel objects
    #   ensures that if this channel is used elsewhere it will continue
    #   to function.
    #
    destructor {
	if {$afterId ne "0"} {
	    after cancel $afterId
	}
	blt::vector destroy ${selfns}::time  ${selfns}::data
	$options(-channel) delete
    }
    #   Peform data update.  This involves getting a new time
    #   relative to the timebase, and updating the epics channel
    #   info and appending these to the vectors we created.
    #   If we are mapped to a line on a strip chart that will
    #   automatically udpate the line.
    #   In addition, if the maxKept value is nonzero, we trim
    #   the vectors to the last maxKept values.
    #
    method updateData {} {
	
	# Get new timebase relative time and epics channel data:
	# Note that epics channels may  not yet be connected:
	# if not, we don't update:
	#

	set now [clock clicks -milliseconds]
	set elapsedMs [expr 1.0*($now - $tZeroMs)]
	set time [expr 1.0*$startSeconds + $elapsedMs/1000.0]

	set lastPeriod $options(-period)


	if {![catch {$options(-channel) get} value]} {
	    
	    ${selfns}::time append $time
	    ${selfns}::data append $value

	    
	    # If necessary trim the data backe to maxKept:
	    # this code assumes the two vectors are the same length
	    # this will only not be true if the programmer has been very
	    # very dirty.
	    
	    if {$maxKept > 0} {
		#
		#  Trim back to a 10% margin on maxkept --
		#  So that we won't be trimming every iteration.
		# 
		set length [${selfns}::time length]

		if {$length > $maxKept} {
		    set toKeep [expr 0.9*$maxKept]
		    set clearCount [expr int($length - $toKeep)]
		    $self clearfirst $clearCount
		}
		    
	    }
	    
	}
	# Schedule the next update:
	   
	set afterId [after $options(-period) [mymethod updateData]]
    }
    #  Make the time and data elements of the vector empty
    #  this 'forgets' the time history on any stripchart on which this history
    #  data is displayed.
    #
    method clear  {} {
	set num [${selfns}::time length]
	$self clearfirst $num

    }
    # Clears the first n elements fromt he time and data vectors.
    #
    method clearfirst n {

	if {![string is integer -strict $n]} {
	    error "clearfirst requires an integer parameter"
	}
	if {$n < 0} {
	    error "clearfirst requires a parameter >= 0."
	}

	if {$n > [${selfns}::time length]} {
	    set n [${selfns}::time length]
	}
	for {set i 0} {$i < $n} {incr i} {
	    lappend indexlist $i
	}
	eval ${selfns}::time delete $indexlist
	eval ${selfns}::data delete $indexlist
    }
    # Sets the maxKept value to n
    # Next time the vector is updated, the 
    # the vector will, if needed be trimmed as requested.
    # n <= 0 means that no trimming will be done (at least not
    # automatically.
    
    method keep n {
	if {![string is integer -strict $n]} {
	    error "keep requires an integer parameter"
	}
	if {$n <= 0} {
	    error "keep requires an integer > 0"
	}
	set maxKept $n
    }
    # Sets the new update period in ms.
    # This takes effect after the next update occurs.
    # (that is we just set options(-period).  Note that an undocumented
    # but perfectly good way to do this is to configure the object -period nn
    # This method will validity check its parameter, however.
    #
    method updateperiod n {
	if {![string is integer -strict $n]} {
	    error "Update periods must be in integer number of ms"
	}
	if {$n <= 0} {
	    error "Update periods must be > 0"
	}

	set options(-period) $n
    }
    #  Returns the data in time/value pairs
    #  This allows strip charts to save their histories !!!
    #  Something the NCSAPPS stripchart cannot now do.
    #
    method get {} {
	set result [list]
	set points [${selfns}::time length]
	for {set i 0} {$i < $points} {incr i} {
	    set time [${selfns}::time range $i $i]
	    set data [${selfns}::data range $i $i]
	    lappend result [list $time $data]
	}
	return $result
    }
    # Return the names of the vectors in the line.
    # This member is not intended to be used by casual clients.
    # The epicsStripChart widget needs this to be able to 
    # construct the final line elements
    #
    method names {} {
	return [list ${selfns}::time ${selfns}::data]
    }
}

#-------------------------------------------------------------------------------
#
#  Now we can implement the strip chart widget in terms of the
#  BLT widget.
#

snit::widget controlwidget::epicsStripChart {
    delegate option * to stripchart
    delegate method * to stripchart

    variable timeBase;		# T=0.
    variable channelLines;	# Array of channelHistoryObject names by channel

    constructor args {
	eval install stripchart as blt::stripchart $win.stripchart $args
	pack $win.stripchart -expand 1 -fill both

	set timeBase [clock seconds]
    }
    #  Add a new channel to the strip chart.
    #  This creates a new channel history object to keep track of the data
    #  The vectors of that object are added as a line to the chart:
    #
    method addchannel {name interval args} {
	if {[array names channelLines $name] ne ""} {
	    error "This channel is already displayed on the stipchart"
	}
	set channelLines($name) [controlwidget::channelHistory %AUTO%  \
				     -period  $interval                \
				     -channel $name                    \
				     -timebase $timeBase]
	set vecs [$channelLines($name) names]
	eval $win.stripchart element  create $channelLines($name)    \
	    -xdata [lindex $vecs 0]                         \
	    -ydata [lindex $vecs 1]                         \
	    -label $name                                    \
	    $args

	return $channelLines($name)
    }
    #  Remove a channel from the strip chart:
    #  The element (line) is destroyed, and the 
    #  channeHistory object is destroyed as well.
    #

    method removechannel name {
	if {[array names channelLines $name] eq ""} {
	    error "Tried to remove $name from a stripchart it's not on: $win"
	}
	set elementName $channelLines($name)
	unset channelLines($name)
	$win.stripchart element delete $elementName
	$elementName destroy
    }
}