# Compile gtest
noinst_LIBRARIES = libgtest.a

GTEST_SRC = \
  gtest/src/gtest-death-test.cc \
  gtest/src/gtest-filepath.cc \
  gtest/src/gtest-internal-inl.h \
  gtest/src/gtest-port.cc \
  gtest/src/gtest-printers.cc \
  gtest/src/gtest-test-part.cc \
  gtest/src/gtest-typed-test.cc \
  gtest/src/gtest.cc \
  gtest/include/gtest/gtest-death-test.h \
  gtest/include/gtest/gtest-message.h \
  gtest/include/gtest/gtest-param-test.h \
  gtest/include/gtest/gtest-printers.h \
  gtest/include/gtest/gtest-spi.h \
  gtest/include/gtest/gtest-test-part.h \
  gtest/include/gtest/gtest-typed-test.h \
  gtest/include/gtest/gtest.h \
  gtest/include/gtest/gtest_pred_impl.h \
  gtest/include/gtest/gtest_prod.h \
  gtest/include/gtest/internal/gtest-death-test-internal.h \
  gtest/include/gtest/internal/gtest-filepath.h \
  gtest/include/gtest/internal/gtest-internal.h \
  gtest/include/gtest/internal/gtest-linked_ptr.h \
  gtest/include/gtest/internal/gtest-param-util-generated.h \
  gtest/include/gtest/internal/gtest-param-util.h \
  gtest/include/gtest/internal/gtest-port.h \
  gtest/include/gtest/internal/gtest-string.h \
  gtest/include/gtest/internal/gtest-tuple.h \
  gtest/include/gtest/internal/gtest-type-util.h

EXTRA_DIST = $(GTEST_SRC)

libgtest_a_SOURCES = gtest/src/gtest-all.cc
libgtest_a_CXXFLAGS = -Igtest -Igtest/include

# Compile unit tests
check_PROGRAMS = unittest
unittest_SOURCES = \
	main.cpp \
	btstreamtest.cpp \
	videobuffertest.cpp \
	videotorrentmanagertest.cpp

unittest_LDADD = \
	$(top_builddir)/src/libbtstream.la \
	libgtest.a \
	@BOOST_THREAD_LIB@ \
	@BOOST_SYSTEM_LIB@

#unittest_LDFLAGS = -no-undefined @BOOST_LDFLAGS@
unittest_CXXFLAGS = \
	-Igtest/include \
	-I$(top_builddir)/src \
	@BOOST_CPPFLAGS@ \
	@LIBTORRENT_CFLAGS@ \
	$(AM_CXXFLAGS)

# Programs run with "make check" 
TESTS = $(check_PROGRAMS)
