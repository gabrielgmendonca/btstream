plugin_LTLIBRARIES = libbtstreamsrc.la

libbtstreamsrc_la_SOURCES = \
  btstreamsrc.cpp \
  btstreamsrc.h \
  propertyinstaller.cpp \
  propertyinstaller.h
   
libbtstreamsrc_la_LDFLAGS = -version-info @INTERFACE_VERSION_INFO@ -no-undefined

libbtstreamsrc_la_LIBADD = @LIBBTSTREAM_LIBS@ @GSTREAMER_LIBS@
libbtstreamsrc_la_CXXFLAGS = @LIBBTSTREAM_CFLAGS@ @GSTREAMER_CFLAGS@ $(AM_CXXFLAGS)
