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

package provide epicsGraph  1.0
package require Tk
package require snit
package require BLT
package require epics

namespace eval controlwidget {}
#============================channelPairHistory snit::type==================
# The snit type channelPairHistory is essentially a channel history
# that couples a pair of channels.  This is used fo x/y channel graphds e.g.
#
# OPTIONS:
#   -period       - ms between updates of the data.
#   -xchannel     - Name of the x channel.
#   -ychanel      - Name of the y channel.
#   -timebase     - [clock seconds] at which the data series start.
#
# METHODS:
#   clear         - clear the data series.
#   clearfirst    - Clear the first n points in a data series.
#   keep          - Only keep at most n data points in the series
#                   enforced at updates.
#  updateperiod   - Set a new update period... configure -period is preferred.
#  get            - Get the data series, a list of time,x,y triplets.
#  names          - Get the name of the time, x and y vectors.
#

snit::type controlwidget::channelPairHistory {
    option -period
    option -xchannel
    option -ychannel
    option -timebase
    
    variable lastPeriod
    variable lastTime
    variable maxKept
    variable afterId   0
    
    constructor args {
        $self configurelist $args
        
        #  Need to have both channels, a period and a reasonable timebase:
        
        #    Validate period:
        
        if {![string is integer -strict $options(-period)]} {
            error "controlwidget::channelPairHistory: Must have an integer -period at construction"
        }
        if {$options(-period) <= 0} {
            error "controlwidget::channelPairHistory: -period must be a non negative integer value"
        }
        #    Validate timebase:
        
        if {![string is integer -strict $options(-timebase)]} {
            error "controlwidget::channelPairHistory -timebase should be the ouptut of e.g. [clock seconds]"
        }
        #     Need to have both channels:
        
        if {($options(-xchannel) eq "") || ($options(-ychannel) eq "")} {
            error "controlwidget::channelPairHistory must be constructed with both -xchannel and -ychannel specified"
        }
        
        # Initialize our data:
        
        set lastPeriod $options(-period)
        set afterId     0
        set maxKept     0
        set lastTime   [expr 1.0*[clock seconds]]    ;#   Floating point seconds.
        
        # Make the vectors we need for the time, x and y data:

   
        blt::vector create ${selfns}::time
        blt::vector create ${selfns}::x
        blt::vector create ${selfns}::y
        
        epicschannel $options(-xchannel)
        epicschannel $options(-ychannel)
        
        # get start the update schedule:

        
        $self updateData
    }
    #------------------------------------------------------------------------
    #
    #  Destructor.. we need to kill off the vectors and the epics channels
    #  as well as stop the after from continuing to propagate.
    #  The epics channels maintain reference counts that ensure the
    #  destroy is safe.. only when all instances of an epicschannel on the same
    # channel are destroyed is the connection actually cut.
    #
    destructor {
        if {$afterId != 0} {
            after cancel $afterId
        }
        
        blt::vector destroy ${selfns}::time ${selfns}::x ${selfns}::y
        $options(-xchannel) delete
        $options(-ychannel) delete
    }
    #---------------------------------------------------------------------
    #
    # updateData - Update the data in the data set... reschedule ourself as well.#
    # Note that if either channel is not yet connected, we don't update the
    # vectors.
    #
    #  regarless, we reschedule ourselves to run again.
    #
    method updateData {} {

        # Get new timebase relative time and epics channel data:

        set time     [expr $lastTime + $lastPeriod/1000.0]
        set lastTime $time
        set lastPeriod $options(-period)
        set time     [expr $time - $options(-timebase)]
    
        # Get the epics data. unconnected will reflect the
        # number of disconnected channels.
    
        set unconnected 0
        if {[catch {$options(-xchannel) get} xdata]} {
            incr unconnected
        }
        if {[catch {$options(-ychannel) get} ydata]} {
            incr unconnected
        }
        
        #  Update the data series only if both epics channels are connected.
        
        if {$unconnected == 0} {
            ${selfns}::time append $time
            ${selfns}::x    append $xdata
            ${selfns}::y     append $ydata
        }

        # Trim back the data as needed to stay within maxKept.
        
        if {$maxKept > 0} {
            while {[${selfns}::time length] > $maxKept} {
                ${selfns}::time delete 0
                ${selfns}::x    delete 0
                ${selfns}::y    delete 0
            }
        }
        #  Schedule the next udpate:
        
        set afterId [after $options(-period) [mymethod updateData]]
    }

    #--------------------------------------------------------------------------
    #
    #   Clear all the points in the data series:
    #
    method clear {} {
        set num [${selfns}::time length]
        $self clearfirst $num
    }


    #-------------------------------------------------------------------------
    #
    #  Clear the first n elements from the time and data vectors:
    #
    method clearfirst n {
        if {![string is integer -strict $n]} {
            error "channelPairHistory::clearfirst requires an integer parameter"
        }
        if {$n < 0} {
            error "channelPairHistory::clearfirst requires a parameter >= 0"
        }
        # Any clears of more than the existing data elements just clear the entire series:
        
        if {$n > [${selfns}::time length]} {
            set n [${selfns}::time length]
        }
        
        #  now get rid of the first n elements:
        
        while {$n} {
            ${selfns}::time delete 0
            ${selfns}::x    delete 0
            ${selfns}::y    delete 0
            
            incr n -1
        }
    }

    #-------------------------------------------------------------------------
    #
    #   Sets the maximum number of data points to keep.  Since we are assuming
    #   this is an active data series, we don't actually need to trim the series,
    #   we'll just let the next update trim it for us.
    #
    method keep n {
        if {![string is integer -strict $n]} {
            error "channelHistoryPair::keep requires an integer parameter"
        }
        if {$n <= 0} {
            error "channelHistoryPair::keep requires an integer > 0"
        }
        set maxKept $n

    }

    #---------------------------------------------------------------------------
    #
    #   Modify the update period... This can also be done perfectly well
    #  with a  configure -period.. however we keep this to retain method level
    #  compatibility with the channelHistory snit::type.
    #
    method updateperiod n {
        if {![string is integer -strict $n]} {
            error "channelPairHistory::updateperiod Update periods must be in integer number of ms"
        }
        if {$n <= 0} {
            error "channelPairHistory::updateperiod Update periods must be > 0"
        }
        set options(-period) $n
    }

    #------------------------------------------------------------------------------
    #
    #    Returns the names of the vectors that we have created.
    #    This allows the epicsgraph widget to e.g. specify them
    #    as data series.  To be complete, we return all three vector names
    #    in the order time, x, y
    #
    method names {} {
        return [list ${selfns}::time ${selfns}::x ${selfns}::y]
    }

    #------------------------------------------------------------------------------
    #
    #   Returns the data series.  The data series is a list of triplets.
    #   each triplet has time,x,y in that order.  The times are relative to the
    #   timebase which can be obtained via $object cget -timebase.
    #
    method get {} {
        set result [list]
        set pointCount [${selfns}::time length]
        for {set i 0} {$i < $pointCount} {incr i} {
            #
            #  For some reason, I can't get indexing to work quite right
            # for vectors in a namespace so:
            
            set time [${selfns}::time range $i $i]
            set x    [${selfns}::x    range $i $i]
            set y    [${selfns}::y    range $i $i]
            
            lappend result [list $time $x $y]
        }
        return $result
    }


}


#============================epicsGraph snit::widget==========================
#
#  This widget is a thin wrapping around a blt::graph widget, so that it is
#  easier to dispplay evolution of pairs of epics channels.
# OPTIONS:
#    All blt::graph options are supported...and delegated to the blt::graph widget.
# METHODS:
#   All blt::graph methods are suported..and delegated to the blt::graph wiget.
#   In addition:
#
#   addseries   name x y interval ?options?
#      Adds a new series named 'name' that plots the x epics channel against the y epics channel
#      A new data point is added every interval milliseconds.  If any ?options? are supplied,
#      they are passed without interpretation to the blt element create method, and can be used
#      to e.g. tailor the visual display of the series.
#   removeseries name
#      Removes the series name created with addseries, and destroys all resources associated with it.
#
#
snit::widgetadaptor  controlwidget::epicsGraph {
    delegate option * to hull
    delegate method * to hull
    
    variable timebase;               # T=0 time.
    variable series;                 # Array of channelPairHistories by series name.

    constructor args {
        eval installhull using  blt::graph  $args
        
        
        set timebase [clock seconds]
    }




    #---------------------------------------------------------------------------
    #
    # Remove a series from the strip chart.. the element is destroyed,
    # and the channelPairHistory is also destroyed.
    # Parameters:
    #    name   - Name of the series to destroy.
    #
    method removeseries name {
	if {[array names series $name] eq ""} {
	    error "controlwidget::epicsGraph::removeseries - $name is not a series"
	}
	set history $series($name)
	unset series($name)
	$win element delete $name
	$history destroy
    }


    #--------------------------------------------------------------------------------
    #
    #  Add a new series based on a pair of epics channels to the plot:
    #
    # Parameters:
    #   name   - Name of the new series created (this will be the element name).
    #   x      - Name of epics process variable to display on the x axis.
    #   y      - Name of epics process variable to display on the y axis.
    #   interval - Update interval.
    #   ?options?- Options to add to the element create command.
    #
    method addseries {name x y interval args} {
	if {[array names series $name] ne ""} {
	    error "controlwidget::epicsGraph::addseries series $name already exists"
	}
	set series($name) [controlwidget::channelPairHistory %AUTO%   \
			       -period $interval                      \
			       -xchannel $x -ychannel $y              \
			       -timebase $timebase]
	set vecs [$series($name) names]
	eval $win element create $name                 \
	    -xdata [lindex $vecs 1]                               \
	    -ydata [lindex $vecs 2]                               \
	    -label $name $args

	return $series($name)
    }
}

