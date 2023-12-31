
# Process this file with autoconf to produce a configure script.
AC_INIT(CTCLChannelCommand.cpp)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)

#    Separate device libraries.

AM_INIT_AUTOMAKE(epicstcl, 2.3-001, fox@nscl.msu.edu, [], [http://www.sourceforge.net/projects/epicstcl])  

#
#   The variable below should track the
#   version number above to ensure that
#   all the .so's get appropriately versioned:
#
SOVERSION="8:1"


#  Checks for parameters:

#  Compute the system type:
#  We can only build on linux-gnu  .. or darwin
#  We also have restrictions  on the versions for the drivers.
#


AC_MSG_CHECKING([operating system ])

AC_CANONICAL_HOST

#
#  Test for pthreads:
# Defines:
#    PTHREAD_LIBS - linker flagbs.
#    PTHREAD_CFLAGS - C/C++ flags.
#    PTHREAD_CC     - Special compiler if requred.
# 
AX_PTHREAD



case $host_os in
    linux-gnu*)
        kernel_full_version="$(uname -r)"
        kernel_version="$(echo $kernel_full_version | cut -f1,2 -d.)"
        AC_MSG_RESULT([Building on $host_os kernel $kernel_version])

        arch="$(uname -m)"
        case $arch in
            i[[3456]]86)
                libsysdir="linux-x86"
                binsysdir="linux-x86"
                ;;
            x86_64)
                libsysdir="linux-x86_64"
                binsysdir="linux-x86_64"
                ;;
            aarch64|armv[[678]]*)
                libsysdir="linux-arm"
                binsysdir="linux-arm"
                ;;
            *)
                echo 'no idea how to deal with this architecture: ' $arch
                exit
                ;;
        esac


        incsysdir="os/Linux"
        OSLDFLAGS="-Wl,\"-rpath=$libdir\""
        SLIBEXT=".so"
	;;
    *darwin*)
        # God help me when the version changes:

        AC_MSG_RESULT([Building on MAC-OSX 8.9.0])
        incsysdir="os/Darwin"
        libsysdir="darwin-ppc"
        binsysdir="darwin-ppc"
        SLIBEXT=".dylib"
 
	;;
    *)
        AC_MSG_ERROR([This software can only be built on linux-gnu or darwin not $host_os])
        ;;
esac


# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_LN_S

# Documentation tools (docbook xmlto):

AC_PATH_PROG(HCDOCBOOK,  docbook2dvi,  echo)
AC_PATH_PROG(HTMLDOCBOOK, xmlto, echo)
AC_PATH_PROG(MANDOCBOOK,  xmlto,        echo)
AC_PATH_PROG(DVIPDF,       dvipdf,        echo)
MANDOCBOOK="$MANDOCBOOK man"

#

AC_PROG_GCC_TRADITIONAL

# Checks for libraries.


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(netdb.h)
AC_CHECK_HEADERS(netinet/in.h)
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(sys/ioctl.h)
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(arpa/inet.h)
AC_CHECK_HEADERS(sys/socket.h)
AC_CHECK_HEADERS(limits.h)
AC_CHECK_HEADERS(stddef.h)


AC_HEADER_MAJOR
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STAT
AC_PATH_XTRA

# Checks for typedefs, structures, and compiler characteristics.

AC_STRUCT_TM
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_TYPE_OFF_T
AC_C_CONST
AC_C_INLINE



# Checks for library functions.

AC_FUNC_MMAP
AC_CHECK_FUNCS(stat)
AC_CHECK_FUNCS(malloc)
AC_CHECK_FUNCS(munmap)
AC_CHECK_FUNCS(getpagesize)
AC_CHECK_FUNCS(memset)
AC_CHECK_FUNCS(strerror)
AC_CHECK_FUNCS(gethostbyname)
AC_CHECK_FUNCS(gethostbyaddr)
AC_CHECK_FUNCS(inet_ntoa)
AC_CHECK_FUNCS(gettimeofday)
AC_CHECK_FUNCS(select)
AC_CHECK_FUNCS(strchr)
AC_CHECK_FUNCS(gethostname)
AC_CHECK_FUNCS(getpass)
AC_CHECK_FUNCS(socket)
AC_CHECK_FUNCS(strdup)
AC_CHECK_FUNCS(strstr)
AC_CHECK_FUNCS(fork)
AC_CHECK_FUNCS(clock_gettime)
AC_CHECK_FUNCS(strtol)
AC_CHECK_FUNCS(strtoul)


#  See if the C runtime libs have .h's on them or
#  if h-less versions can be used for what we need:
#

AC_LANG_PUSH(C++)
AC_CHECK_HEADER(iostream, [AC_DEFINE([HAVE_HLESS_IOSTREAM],1,[<iostream> exists])],[])
AC_CHECK_HEADER(istream,  [AC_DEFINE([HAVE_HLESS_ISTREAM],1,[<istream> exists])],[])
AC_CHECK_HEADER(ostream,  [AC_DEFINE([HAVE_HLESS_OSTREAM],1,[<ostream> exists])],[])
AC_CHECK_HEADER(streambuf,[AC_DEFINE([HAVE_HLESS_STREAMBUF],1,[<streambuf> exists])],[])
AC_CHECK_HEADER(fstream,  [AC_DEFINE([HAVE_HLESS_FSTREAM],1,[<fstream> exists])],[])
AC_CHECK_HEADER(iomanip,  [AC_DEFINE([HAVE_HLESS_IOMANIP],1,[<iomanip> exists])],[])
AC_CHECK_HEADER(sstream,  [AC_DEFINE([HAVE_HLESS_SSTREAM],1,[<sstream> exists])],[])
AC_CHECK_HEADER(typeinfo, [AC_DEFINE([HAVE_HLESS_TYPEINFO],1,[<typeinfo> exists])],[])



#   Is there an std namespace to import from?

AC_COMPILE_IFELSE([using namespace std;], [AC_DEFINE([HAVE_STD_NAMESPACE],[1],[1 if std namespace defined])])
AH_VERBATIM([VHAVE_STD_NAMESPACE],[
/* Define STD macro according to HAVE_STD_NAMESPACE value */
#ifdef HAVE_STD_NAMESPACE
#define STD(name)  std::name
#else
#define STD(name) name
#endif])



AC_LANG_POP(C++)

#  Check for Tcl  This test produces:
#  TCL_DEFS     - definitions used to compile Tcl.
#  TCL_LIBS     - Link library switches needed to link Tcl.
#  TCL_CPPFLAGS - C/C++ compilation flags.
#  TCLSH        - tclsh shell.
#
#  WISH         - wish Tk shell.
#  TK_DEFS      - Definitions used to compile Tk.
#  TK_LIBS      - link time flags to link with Tk.
#  TK_CPPFLAGS  - Preprocessor flags to compile C/C++ with Tcl/Tk.

AX_TCL

# Check for CPPUNIT - C++ Unit test framework.  This defines:
#
# CPPUNIT_CFLAGS - C/C++ compiler flags.
# CPPUNIT_LIBS   - Loader flags that pull in the library.

AM_PATH_CPPUNIT

# Assume that CPPUNIT_LIBS is not null if it was found:

AM_CONDITIONAL([CPPUNIT], [test -n "${CPPUNIT_LIBS}"])

#  X11 libraries 

X11INCS="${X_CFLAGS}"
X11LIBS="${X_LIBS} ${X_EXTRA_LIBS} -lXm -lXt -lX11"

##
#  Figure out libtclplus. If this is installed in a standardized
#  place use it, otherwise configure make clean install
#  the bundled version in the libtcl directory.
#  Produce:
#     LIBTCLPLUS_CFLAGS - Compilation flags required to build against the
#                         appropriate one.
#     LIBTCLPLUS_LDFLAGS - Appropriate linker flags.
#
# 

AC_CHECK_LIB(Tcl_AppInit, tclPlus, [systemtcl="yes"], [systemtcl="no"])


#
#  IF systemtcl is yes we're pretty much done.. otherwise
#  we need to configure/build/install libtcl into prefix and then set
#  the flags:

AC_MSG_CHECKING([Checking for libtclPlus])
if test $systemtcl = "yes"
then
	LIBTCLPLUS_CFLAGS=
	LIBTCLPLUS_LDFLAGS=-ltclPlus -lException
	AC_MSG_RESULT([Found in  system libraries])
else
	AC_MSG_RESULT([Need to build local tagged copy])
	(cd libtcl; ./configure --prefix=${prefix}; make clean install)
	LIBTCLPLUS_CFLAGS="-I${prefix}/include"
	LIBTCLPLUS_LDFLAGS=-"L${prefix}/lib -ltclPlus -lException -Wl,\"-rpath=${prefix}/lib\""

fi

AC_SUBST(LIBTCLPLUS_CFLAGS)
AC_SUBST(LIBTCLPLUS_LDFLAGS)



#
#   Look for epics in:
#    /usr/local/epics
#    /usr/lib/epics   [BNL].
#    --with-epics-rootdir=
# Underneath that we have (we require intel linux remember):
#    include                  - the include directory
#    include/os/$incsysdir     - os specific includes.
#    lib/$libsysdir            - libs
#    bin/linux-x86/caRepeater - The caRepeater program.
#

epicstestdirs="/usr/local/epics /usr/lib/epics"
haveepics="no"

AC_MSG_CHECKING([ for epics includes, libs and binaries])

#  Check for --with-epics-rootdir....

AC_ARG_WITH(epics-rootdir,
	[  --with-epics-rootdir=top level directory of EPICS install],
	[haveepics="yes"
	 epicsroot="$withval"],
	[haveepics="no"])



if test $haveepics == "no"
then
   for dir in $epicstestdirs
   do
      if test $haveepics == "no"
      then
        AC_CHECK_FILE([${dir}/include/cadef.h],
	              [haveepics="yes"
                       epicsroot=$dir])
      fi
   done
fi

#  If we found epics we can build the software that needs it.
#  otherwise it's optional.

if test $haveepics == "yes"
then
	# Have epics -- assuming gcc for now -- at some point epics adds a ...compiler/compiler-name
	# that must be included.


   EPICSSOFTWARE="controlpush epicsdisplay chanlog" # Add these directories to build.
   EPICS_INCLUDES="-I$epicsroot/include -I$epicsroot/include/$incsysdir -I$epicsroot/include/compiler/gcc"
   EPICS_LDFLAGS="-L$epicsroot/lib/$libsysdir -lca -lCom -lrt"
   if test "$host_os" == "linux-gnu"
   then
      EPICS_LDFLAGS="$EPICS_LDFLAGS  -Wl,\"-rpath=$epicsroot/lib/$libsysdir\""
   fi
   EPICS_BIN="-DEPICS_BIN=\"\\\"$epicsroot/bin/$binsysdir\\\"\""
else
	# No epics.. provide blank variables:

   EPICSSOFTWARE=""	# no directories to add to make.
   EPICS_LDFLAGS=""	# No ldflags.	
   EPICS_INCLUDES=""	# No includes.
   EPICS_BIN=""
fi




# define substitutions:

#  Go for a strict linguistic interpretation:

#       CXXFLAGS="-std=c++98 -pedantic"

#   someday:   AC_SUBST(CXXFLAGS)

AC_SUBST(OSLDFLAGS)
AC_SUBST(SLIBEXT)

	# EPICS if it exists.

AC_SUBST(EPICSSOFTWARE)
AC_SUBST(EPICS_LDFLAGS)
AC_SUBST(EPICS_INCLUDES)
AC_SUBST(EPICS_BIN)



	# X11: 

AC_SUBST(X11LIBS)
AC_SUBST(X11INCS)



# Documentation tools

AC_SUBST(HCDOCBOOK)
AC_SUBST(MANDOCBOOK)
AC_SUBST(HTMLDOCBOOK)
AC_SUBST(DVIPDF)



#  Create output files:

AC_OUTPUT(Makefile)
