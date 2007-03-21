# Make a man page from xml... note that the
# touch is done just in case the docbook man page creation
# command does not exist.


# DEFS = @WCHAR_T_DEF@
INCLUDES = -I.. -I. @TCL_FLAGS@ -I../Headers -I../Exception @X11INCS@
LDADD = -ltclPlus -lException -lXt -lX11 $(X11EXTRA_LDFLAGS)
AM_LDFLAGS = -L. @TCL_LDFLAGS@

lib_LTLIBRARIES = libtclPlus.la






include_HEADERS =


XMLMANSRC

XMLMANS=

manpages: $(XMLMANS) $(XMLMANSRC)

install-data-local: manpages
	for f in  $(XMLMANS); do                          \
            if [ -e $$f ]; then                                \
		$(INSTALL_DATA) $$f @prefix@/man/man3;         \
	   fi;						      \
	done



man_MANS = TracingIntro.3 CTracedVariable.3  CVariableTraceCallback.3 \
	 CLanguageTraceCallback.3

EXTRA_DIST = TCL.WC TCL.omt tkAppInit.c $(man_MANS) $(XMLMANSRC)

if CPPUNIT

noinst_PROGRAMS = tracetests interptests

noinst_HEADERS = Asserts.h



tracetests_SOURCES = TestRunner.cpp \
			ccallbacktest.cpp \
			tracevartest.cpp

tracetests_DEPENDENCIES = $(testableObjects)

TestLdflags = $(testableObjects) $(CPPUNIT_LDFLAGS) $(X11EXTRA_LDFLAGS) \
	-L@top_srcdir@/Exception -lException


tracetests_LDADD = $(TestLdflags)

interptests_SOURCES=interpretertest.cpp objecttest.cpp resulttest.cpp \
	objcommandtest.cpp compatcommand.cpp


interptests_LDADD = TestRunner.o $(TestLdflags)


interptests_DEPENDENCIES = $(testableObjects) TestRunner.o


TESTS = ./tracetests ./interptests

endif
