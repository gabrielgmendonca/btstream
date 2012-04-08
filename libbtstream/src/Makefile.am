lib_LTLIBRARIES = libbtstream.la

libbtstream_la_SOURCES = \
  BTStream.cpp \
  Exception.cpp \
  PiecePicker.cpp \
  SequentialPiecePicker.cpp \
  VideoBuffer.cpp \
  VideoPeerPlugin.cpp \
  VideoTorrentManager.cpp \
  VideoTorrentPlugin.cpp
   
pkginclude_HEADERS = \
  BTStream.h \
  Exception.h \
  PiecePicker.h \
  SequentialPiecePicker.h \
  VideoBuffer.h \
  VideoPeerPlugin.h \
  VideoTorrentManager.h \
  VideoTorrentPlugin.h

libbtstream_la_LDFLAGS = -version-info @INTERFACE_VERSION_INFO@ -no-undefined

libbtstream_la_LIBADD = @LIBTORRENT_LIBS@
libbtstream_la_CXXFLAGS = @LIBTORRENT_CFLAGS@ $(AM_CXXFLAGS)
