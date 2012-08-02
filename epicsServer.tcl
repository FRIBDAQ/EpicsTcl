#!/usr/bin/tclsh

set epicsTclPath /usr/local/lib


#  Packages

package require cmdline
package require csv

lappend auto_path $epicsTclPath

#-------------------------------------------------------------------------
#
#  Global variables:

set tick        0;		        # number of .1 secs the server has run.
set timeout    "";			# Number of ticks until a request times out.
set serverPort "";	  	        # Port on which the server will listen

set       requestIndex  0;	        # next request index.  
array set pendingRequests [list];        # array of requests that are pending. 
array set knownChannels   [list];	# Set of channels we are monitoring.
array set values          [list];	# Array of values.
array set units           [list];	# array of units.

#------------------------------------------------------------------------
#
#  Process timeouts.  
#  Loops over all of the requests and
#  completes those that could not be completed within
#  the timeout interval.
#
proc processTimeouts {} {
    foreach reqidx [array names ::pendingRequests] {
	set request   $::pendingRequests($reqidx)
	set startTime [lindex $request 1]
	if {($::tick - $startTime) > $::timeout} {
	    completeRequest $reqidx
	}
	
    }

}

#----------------------------------------------------------------------
#
# Set up timed processing.
#

proc ticker interval {
    incr ::tick
    after $interval [list ticker $interval]
}
ticker 100

#--------------------------------------------------------------------
#
#  Option processing
#  we support a pair of options: 
#  -p   Specifies the listen port.
#  -t   Specifies the timeout in .1 seconds.
#  -b   Specifies the parameter for the -myaddr in the socket creation.
set options {
    {p.arg    1234   "Set the port on which epicsServer listens for connections"}
    {t.arg    10     "10'ths of a second in a timeout."}
    {e.arg           "Epics Tcl library path"}
    {b.arg    127.0.0.1 "Bind address for the server"}
}



if {[catch {array set switches [cmdline::getoptions argv $options]} msg] == 1} {
    puts $msg
    exit -1
}



set timeout    $switches(t)
set serverPort $switches(p)
set bindAddress $switches(b)
#
#  optionally specified epics lib path:
#
if {$switches(e) ne ""} {
    lappend auto_path $switches(e)
}
package require epics;		# Now we can load epics now that all paths are in.





#--------------------------------------------------------------------
#
#  Connection and request handling
#

# Read a request from the client socket.
# - Read a line from the socket.
# - disable the file event.
# - split the CSV line and 
# - add missing channels to the list of monitored channels.
# - queue the request in the list of pending requests.
# - satisfy all requests that can be satisfied.
# Parameter:
#    socket   - Fd of the socket from which we will read the request.

#
proc readRequest socket {

    set request [gets $socket]
    fileevent $socket readable ""

    # If I got nothing it's either an error in the request or
    # the socket closed by the peer before sending the request line.

    if {$request eq ""} {
	close $socket
	return
    }

    # Decode the request.

    set status [catch {
	
	set request [csv::split $request]
	
	# Add missing channels to the monitored list.
	
	monitorChannels $request
	
	# Create the request list and insert it in pendingRequests.
	
	set requestList [list $socket $::tick $request]
	set ::pendingRequests($::requestIndex) $requestList
	incr ::requestIndex
	
	# See if the request and others can be satisfied:
	
	satisfyRequests
    } msg]
    # Any error merits closing the socket. now.
    #
    if {$status} {
	puts "Error in request: $msg $::errorInfo"
	close $socket
	return
    }
}
#
#  Satisfy any requests that have all their data present
#
#
proc satisfyRequests {} {
    foreach index [array names ::pendingRequests] {
	set channels [lindex $::pendingRequests($index) 2]
	
	# Complete the request if all channel values and units are there.
	
	set allThere 1
	foreach channel $channels {
	    if {([array names ::values $channel] eq "") ||
		([array names ::units  $channel]  eq "") ||
		($::knownChannels($channel) ne "connected")} {
		set allThere 0
		break
	    } 

	}
	if {$allThere} {
	    completeRequest $index
	}
    }
}

#  Satisfy a single request with the data that is now available..providing
#  empty elements if data are not available.
# Parameter:
#   request - a request index from the set of elements in 
#             pendingRequests.
# NOTE:
#   when execution is complete this element will be removed from pendingRequests.
#
proc completeRequest index {
    set request $::pendingRequests($index)
    unset ::pendingRequests($index)

    set socket   [lindex $request 0]
    set channels [lindex $request 2]

    # Fetch the data ..unavailable data is put in as empty elements.

    set result [list]
    foreach channel $channels {
	if {([array names ::values $channel] eq "")    ||
	    ([array names ::units $channel] eq "")} {
	    lappend result "" ""
	} else {
	    lappend result $::values($channel) $::units($channel)
	}
    }
    #
    # Send the data back to the client.
    #

    set line [csv::join $result]
    puts $socket $line
    flush $socket
    close $socket

}

# Process a connection request.  To do this we
# Set up socket readability to invoke readRequest with 
# line buffering on the socket.
#
# Parameters:
#   socket   - The data transfer socket.
#   address  - Client ip address.
#   port     - Client port number.
#
proc processConnection {socket address port} {

    # set the socket to line buffering then set a fileevent on socket readability.
    #

    fconfigure $socket -buffering line
    fileevent $socket readable [list readRequest $socket]


}


#   Set us up as a server.

socket -server processConnection -myaddr $bindAddress $serverPort

#---------------------------------------------------------------------
#
#  Channel management
#

#
#  Add any un-monitored channels in a list of channels to the list of
#  channels being monitored.
# Parameters:
#   channelList - List of channels to add.
#
proc monitorChannels channelList {
    foreach channel $channelList {
	if {[array names ::knownChannels $channel] eq "" } {
	    epicschannel $channel
	    epicschannel $channel.EGU
	    set ::knownChannels($channel) ""
	    $channel link values($channel)
	    $channel.EGU link units($channel)
	}
    }
}
#
#  Trace fired when an epics variable or units updates.
#  our action is to just attempt to process any pending requests..
#
# Parameters:
#   name  - Name of the variable modified.
#   index - Array index element modified (channel name).
#   op    - Operation performed.
# Return:
#   empty string
#
proc epicsUpdate {name index op} {

    # need 
    catch {
    if {([array names ::values $index] ne "")   &&
	([array names ::units $index]  ne "")} {
	if {$::knownChannels($index) eq ""} {
	    set ::knownChannels($index) 1
	} elseif {$::knownChannels($index) eq 1} {
	    set ::knownChannels($index) 2
	} else {
	    set ::knownChannels($index) "connected"
	}
    }
    } msg

	    
    satisfyRequests

    return ""

}


trace add variable values write epicsUpdate
trace add variable units  write epicsUpdate



#---------------------------------------------------------------------
# Main loop.
#


while 1 {
    vwait tick
    processTimeouts
}
