lib_LTLIBRARIES = libException.la libtclPlus.la libepics.la


INCLUDES= $(THREADFLAGS) @TCL_FLAGS@  @EPICS_BIN@ @EPICS_INCLUDES@ \
	-DTCL_THREADS
### libException

libException_la_SOURCES = ErrnoException.cpp Exception.cpp \
			  RangeError.cpp StreamIOError.cpp \
			StateException.cpp
include_HEADERS = ErrnoException.h Exception.h  StreamIOError.h \
	RangeError.h StateException.h                           \
	 TCLApplication.h TCLCommandPackage.h TCLException.h \
	TCLFileHandler.h TCLHashTable.h TCLHashTableItem.h \
	TCLHashTableIterator.h TCLIdleProcess.h	TCLInterpreter.h \
	TCLInterpreterObject.h TCLList.h TCLObject.h TCLPackagedCommand.h \
	TCLProcessor.h TCLResult.h TCLString.h TCLTimer.h TCLVariable.h \
	TCLChannel.h TCLVersionHacks.h TCLTracedVariable.h \
	VariableTraceCallback.h CLanguageTraceCallbacks.h \
	TCLObjectProcessor.h TCLCompatibilityProcessor.h histotypes.h \
	CTCLChannelCommand.h CTCLEpicsCommand.h \
	CTCLEpicsPackage.h	\
	CChannel.h CChannelVariable.h

libException_la_LDFLAGS = $(THREADFLAGS) -version-info 2:0:0 \
	$(OSLDFLAGS)



# libtclPlus:

libtclPlus_la_LDFLAGS = $(THREADFLAGS) -version-info 2:0:0 \
	$(OSLDFLAGS)

libtclPlus_la_LIBADD = @top_srcdir@/libException.la 



testableSources = TCLInterpreter.cpp TCLProcessor.cpp  \
	TCLVariable.cpp TCLString.cpp  \
	TCLResult.cpp TCLTimer.cpp TCLFileHandler.cpp \
	TCLList.cpp TCLException.cpp TCLInterpreterObject.cpp \
	TCLCommandPackage.cpp TCLIdleProcess.cpp \
	TCLObject.cpp TCLChannel.cpp TCLTracedVariable.cpp \
	CLanguageTraceCallbacks.cpp TCLObjectProcessor.cpp \
	TCLCompatibilityProcessor.cpp 

testableObjects = TCLInterpreter.o   TCLProcessor.o    \
	TCLVariable.o   TCLString.o    \
	TCLResult.o   TCLTimer.o   TCLFileHandler.o   \
	TCLList.o   TCLException.o \
	TCLInterpreterObject.o   \
	TCLCommandPackage.o   TCLIdleProcess.o   \
	TCLObject.o   TCLChannel.o   TCLTracedVariable.o   \
	CLanguageTraceCallbacks.o  TCLObjectProcessor.o \
	TCLCompatibilityProcessor.o

libtclPlus_la_SOURCES = TCLApplication.cpp CChannelVariable.cpp \
	$(testableSources)



###### epics loadble Tcl package:


libepics_la_SOURCES=CTCLChannelCommand.cpp CTCLEpicsCommand.cpp \
			CTCLEpicsPackage.cpp CChannel.cpp

libepics_la_LIBADD  = @top_srcdir@/libtclPlus.la  \
			@top_srcdir@/libException.la \
			$(TCL_LDFLAGS) $(EPICS_LDFLAGS)		

libepics_la_LDFLAGS = 	$(THREADFLAGS) -version-info 2:0:0 $(OSLDFLAGS)




####### The Tcl Widgets.

PKGDIR=$(prefix)/TclLibs/widgets

PACKAGES = meter.tcl bcmMeter.tcl epicsLabel.tcl \
	radioMatrix.tcl epicsEnumeratedControl.tcl \
	typeNGo.tcl epicsTypeNGo.tcl		\
	epicsLabelWithUnits.tcl bindDown.tcl led.tcl \
	epicsLed.tcl epicsspinbox.tcl epicsButton.tcl \
	epicsMeter.tcl epicsStripChart.tcl epicsgraph.tcl \
	pollmanager.tcl epicsPullDown.tcl

# Man pages:

WEBXMLFILES= manual.xml bcmMeter.xml epicsLabel.xml	\
		 epicsEnumeratedControl.xml	\
		 epicsTypeNGo.xml epicsLabelWithUnits.xml \
		epicsLed.xml epicsspinbox.xml epicsButton.xml \
		epics.xml epicsMeter.xml epicsBCMMeter.xml \
		epicsStripChart.xml epicsgraph.xml epicsPullDown.xml

XMLFILES = manual.xml meter.xml \
	radioMatrix.xml \
	typeNGo.xml      \
	led.xml  \
	CTCLApplication.xml CTCLInterpreterObject.xml \
	CTCLInterpreter.xml CTCLVariable.xml CTCLChannel.xml \
	CTCLCommandPackage.xml CTCLCompatibilityProcessor.xml \
	CTCLException.xml CTCLFileHandler.xml CTCLHashTable.xml \
	CTCLHashTableItem.xml CTCLHashTableIterator.xml CTCLIdleProcess.xml \
	CTCLList.xml CTCLObject.xml CTCLObjectProcessor.xml \
	CTCLPackagedCommand.xml CTCLProcessor.xml CTCLResult.xml \
	CTCLString.xml CTCLTimer.xml TclPlus.xml \
	$(WEBXMLFILES)


MANPAGES =  intro.1tcl bcmMeter.1tcl epicsLabel.1tcl \
	 epicsEnumeratedControl.1tcl \
	epicsTypeNGo.1tcl epicsLabelWithUnits.1tcl \
	epicsLed.1tcl epicsspinbox.1tcl epicsButton.1tcl \
	epics.3tcl epicsMeter.1tcl \
	epicsBCMMeter.1tcl epicsStripChart.1tcl epicsgraph.1tcl

WEBDOCS = epics.pdf

$(WEBDOCS): $(WEBXMLFILES)
	$(HCDOCBOOK) manual.xml
	$(DVIPDF)     manual.dvi
	mv manual.pdf epics.pdf



$(MANPAGES): $(WEBXMLFILES)
	$(MANDOCBOOK) manual.xml
	touch $(MANPAGES)


man_MANS = $(MANPAGES)



#### installation time stuff.

install-exec-local: $(MANPAGES)
	$(mkinstalldirs) $(PKGDIR) 
	chmod o+rx $(PKGDIR)
	$(INSTALL_SCRIPT) $(PACKAGES) $(PKGDIR)
	echo "pkg_mkIndex $(PKGDIR) *.tcl" >makeindex.tcl
	$(TCLSH_CMD) <makeindex.tcl


#  This is to get the epics library put in the right place after install:

install-data-local:
	$(mkinstalldirs) $(prefix)/TclLibs
	-cp $(prefix)/lib/libepics* $(prefix)/TclLibs
	echo "package ifneeded epics 2.0 [list load [file join \$$dir libepics$(SLIBEXT)]]"  \
		>> $(prefix)/TclLibs/pkgIndex.tcl;



clean-local:
	-rm -f $(MANPAGES)



webdocs: $(WEBDOCS)
	$(mkinstalldirs) $(WEBDOCDIR)
	$(INSTALL_DATA)  $(WEBDOCS) $(WEBDOCDIR)


## Tests:

if CPPUNIT
noinst_PROGRAMS = unittests tracetests interptests

unittests_SOURCES = TestRunner.cpp testException.cpp testErrnoException.cpp \
	testStreamIOError.cpp testRangeError.cpp

unittests_LDFLAGS = $(THREADFLAGS)
unittests_LDADD   =  $(CPPUNIT_LDFLAGS) -lException $(TCL_LDFLAGS)

noinst_HEADERS = Asserts.h

tracetests_SOURCES = TestRunner.cpp \
			ccallbacktest.cpp \
			tracevartest.cpp

tracetests_DEPENDENCIES = $(testableObjects)

TestLdflags =  $(testableObjects) $(CPPUNIT_LDFLAGS) $(X11EXTRA_LDFLAGS) \
	-L@top_srcdir@/Exception -lException $(TCL_LDFLAGS)


tracetests_LDFLAGS= $(THREADFLAGS)
tracetests_LDADD = $(TestLdflags)

interptests_SOURCES=interpretertest.cpp objecttest.cpp resulttest.cpp \
	objcommandtest.cpp compatcommand.cpp


interptests_LDADD = TestRunner.o $(TestLdflags) $(TCL_LDFLAGS)
interptests_LDFLAGS=$(THREADFLAGS)

interptests_DEPENDENCIES = $(testableObjects) TestRunner.o



TESTS=./unittests ./tracetests ./interptests
endif

## Tarball generation stuff:

EXTRA_DIST= $(PACKAGES) $(XMLFILES)
