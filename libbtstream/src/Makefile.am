lib_LTLIBRARIES = libbtstream.la

libbtstream_la_SOURCES = \
  btstream.cpp \
  exception.cpp \
  piecepicker.cpp \
  sequentialpiecepicker.cpp \
  videobuffer.cpp \
  videopeerplugin.cpp \
  videotorrentmanager.cpp \
  videotorrentplugin.cpp
   
pkginclude_HEADERS = \
  btstream.h \
  exception.h \
  piecepicker.h \
  sequentialpiecepicker.h \
  videobuffer.h \
  videopeerplugin.h \
  videotorrentmanager.h \
  videotorrentplugin.h

libbtstream_la_LDFLAGS = -version-info @INTERFACE_VERSION_INFO@ -no-undefined @BOOST_LDFLAGS@

libbtstream_la_LIBADD = @BOOST_THREAD_LIB@ @LIBTORRENT_LIBS@
libbtstream_la_CXXFLAGS = @BOOST_CPPFLAGS@ @LIBTORRENT_CFLAGS@ $(AM_CXXFLAGS)
