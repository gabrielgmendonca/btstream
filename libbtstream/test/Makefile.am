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

SUBDIRS=gtest