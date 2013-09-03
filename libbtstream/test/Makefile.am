if ENABLE_TESTS
check_PROGRAMS = unittest
endif

unittest_SOURCES = \
	main.cpp \
	btstreamtest.cpp \
	videobuffertest.cpp \
	videotorrentmanagertest.cpp \
	constants.cpp \
	constants.h

unittest_LDADD = \
	$(top_builddir)/src/libbtstream.la \
	gtest/libgtest.a \
	@BOOST_THREAD_LIB@ \
	@BOOST_SYSTEM_LIB@ \
	@LIBTORRENT_LIBS@

unittest_CXXFLAGS = \
	-Igtest/include \
	-I$(top_builddir)/src \
	@BOOST_CPPFLAGS@ \
	@LIBTORRENT_CFLAGS@ \
	$(AM_CXXFLAGS)

# Programs run with "make check" 
TESTS = $(check_PROGRAMS)

EXTRA_DIST = testfile1 testfile2 testfile1.torrent testfile2.torrent

SUBDIRS=gtest
