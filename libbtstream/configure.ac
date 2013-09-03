# Prelude
AC_PREREQ([2.59])

AC_INIT([libbtstream], [0.2], [gabrielgmendonca@poli.ufrj.br])
AC_CONFIG_MACRO_DIR([m4])

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

m4_define([VERSION_INFO_CURRENT],[2])
m4_define([VERSION_INFO_REVISION],[2])
m4_define([VERSION_INFO_AGE],[0])
INTERFACE_VERSION_INFO=VERSION_INFO_CURRENT:VERSION_INFO_REVISION:VERSION_INFO_AGE
AC_SUBST(INTERFACE_VERSION_INFO)
###############################################################################

# Check system type
AC_CANONICAL_SYSTEM

# Check for programs
AC_PROG_CXX
PKG_PROG_PKG_CONFIG([0.20])

# Check libraries
PKG_CHECK_MODULES([LIBTORRENT], [libtorrent-rasterbar >= 0.16])
AC_SUBST([LIBTORRENT_LIBS])
AC_SUBST([LIBTORRENT_CFLAGS])

AX_BOOST_BASE([1.36])
AX_BOOST_THREAD()

# Prepare optional unit test compilation
AC_ARG_ENABLE(
    [tests],
    [AS_HELP_STRING(
        [--enable-tests],
        [build unit test files [default=no]])],
    [[ARG_ENABLE_TESTS=$enableval]],
    [[ARG_ENABLE_TESTS=no]]
)

AS_IF([test "x$ARG_ENABLE_TESTS" = "xyes"], [AX_BOOST_SYSTEM()])

AM_CONDITIONAL([ENABLE_TESTS], [test "x$ARG_ENABLE_TESTS" = "xyes"])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Initialize Libtool
LT_PREREQ([2.2.6])
LT_INIT

# Output files
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile test/gtest/Makefile libbtstream.pc])
AC_OUTPUT

