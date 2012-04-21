# Prelude
AC_PREREQ([2.59])

AC_INIT([btstreamsrc], [0.1], [gabrielgmendonca@poli.ufrj.br])
AC_CONFIG_MACRO_DIR([m4])

dnl Get prefix from GStreamer 
AC_PREFIX_PROGRAM(gst-launch-0.10)

AC_LANG_CPLUSPLUS

###############################################################################
dnl ---------------------------------------------------------------------------
dnl interface version info
dnl ---------------------------------------------------------------------------
dnl Advanced information about versioning:
dnl   * "Writing shared libraries" by Mike Hearn
dnl         http://navi.cx/~mike/writing-shared-libraries.html
dnl   * libtool.info chapter "Versioning"
dnl   * libtool.info chapter "Updating library version information"
dnl ---------------------------------------------------------------------------
dnl Versioning:
dnl  - CURRENT (Major):  Increment if the interface has changes. AGE is always
dnl                      *changed* at the same time.
dnl  - AGE (Micro):      Increment if any interfaces have been added; set to 0
dnl                      if any interfaces have been removed. Removal has
dnl                      precedence over adding, so set to 0 if both happened.
dnl                      It denotes upward compatibility.
dnl  - REVISION (Minor): Increment any time the source changes; set to
dnl                      0 if you incremented CURRENT.
dnl
dnl  To summarize. Any interface *change* increment CURRENT. If that interface
dnl  change does not break upward compatibility (ie it is an addition),
dnl  increment AGE, Otherwise AGE is reset to 0. If CURRENT has changed,
dnl  REVISION is set to 0, otherwise REVISION is incremented.
dnl ---------------------------------------------------------------------------

m4_define([VERSION_INFO_CURRENT],[0])
m4_define([VERSION_INFO_REVISION],[0])
m4_define([VERSION_INFO_AGE],[0])
INTERFACE_VERSION_INFO=VERSION_INFO_CURRENT:VERSION_INFO_REVISION:VERSION_INFO_AGE
AC_SUBST(INTERFACE_VERSION_INFO)
###############################################################################

# Check system type
AC_CANONICAL_SYSTEM

# Check for programs
AC_PROG_CXX
PKG_PROG_PKG_CONFIG([0.20])

# Check for libraries
PKG_CHECK_MODULES([LIBBTSTREAM], [libbtstream])
AC_SUBST([LIBBTSTREAM_LIBS])
AC_SUBST([LIBBTSTREAM_CFLAGS])

PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.10 >= 0.10.35 gstreamer-base-0.10 >= 0.10.35])
AC_SUBST([GSTREAMER_LIBS])
AC_SUBST([GSTREAMER_CFLAGS])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Initialize Libtool
LT_PREREQ([2.2.6])
LT_INIT

# Set plugin install path
GST_MAJORMINOR=0.10
AC_SUBST(GST_MAJORMINOR)

AG_GST_SET_PLUGINDIR

# Output files
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

