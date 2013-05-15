lib_LTLIBRARIES =  libepics.la

ACLOCAL_AMFLAGS= -I m4

INCLUDES= @PTHREAD_CFLAGS@ @CPPUNIT_CFLAGS@ @TCL_CPPFLAGS@  @TK_CPPFLAGS@ @EPICS_BIN@ @EPICS_INCLUDES@ \
	-DTCL_THREADS @LIBTCLPLUS_CFLAGS@


###### epics loadble Tcl package:


noinst_HEADERS = CTCLChannelCommand.h CTCLEpicsCommand.h \
	CTCLEpicsPackage.h	\
	CChannel.h CChannelVariable.h Asserts.h





libepics_la_SOURCES=CTCLChannelCommand.cpp CTCLEpicsCommand.cpp \
			CTCLEpicsPackage.cpp CChannel.cpp  CChannelVariable.cpp

libepics_la_LIBADD  =   @LIBTCLPLUS_LDFLAGS@ \
			@TK_LIBS@ @TCL_LIBS@  $(EPICS_LDFLAGS)		

libepics_la_LDFLAGS = 	@PTHREAD_LIBS@ -version-info 2:2:0 $(OSLDFLAGS)




####### The Tcl Widgets.

PKGDIR=@prefix@/TclLibs/widgets

PACKAGES = meter.tcl bcmMeter.tcl epicsLabel.tcl \
	radioMatrix.tcl epicsEnumeratedControl.tcl \
	typeNGo.tcl epicsTypeNGo.tcl		\
	epicsLabelWithUnits.tcl bindDown.tcl led.tcl \
	epicsLed.tcl epicsspinbox.tcl epicsButton.tcl \
	epicsMeter.tcl epicsStripChart.tcl epicsgraph.tcl \
	pollmanager.tcl epicsPullDown.tcl

TCLPROGRAMS=epicsServer.tcl

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
	$(mkinstalldirs) $(PKGDIR) @bindir@
	chmod o+rx $(PKGDIR)
	$(INSTALL_SCRIPT) $(PACKAGES) $(PKGDIR)
	echo "pkg_mkIndex $(PKGDIR) *.tcl" | @TCLSH@
	$(INSTALL_SCRIPT) $(TCLPROGRAMS) @bindir@


#  This is to get the epics library put in the right place after install:

install-data-local:
	$(mkinstalldirs) @prefix@/TclLibs
	-cp @libdir@/libepics* @prefix@/TclLibs
	echo "package ifneeded epics 2.2 [list load [file join \$$dir libepics$(SLIBEXT)]]"  \
		>> @prefix@/TclLibs/pkgIndex.tcl;



clean-local:
	-rm -f $(MANPAGES)



webdocs: $(WEBDOCS)
	$(mkinstalldirs) $(WEBDOCDIR)
	$(INSTALL_DATA)  $(WEBDOCS) $(WEBDOCDIR)



## Tarball generation stuff:

EXTRA_DIST= $(PACKAGES) $(XMLFILES) $(TCLPROGRAMS) libtcl
