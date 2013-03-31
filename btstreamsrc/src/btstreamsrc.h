/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2011-2013 Gabriel Mendonça <gabrielgmendonca@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_BTSTREAM_SRC_H__
#define __GST_BTSTREAM_SRC_H__

#include <gst/gst.h>
#include <gst/base/gstpushsrc.h>

#include "libbtstream/btstream.h"

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_BTSTREAM_SRC \
  (gst_btstream_src_get_type())
#define GST_BTSTREAM_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_BTSTREAM_SRC,GstBTStreamSrc))
#define GST_BTSTREAM_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_BTSTREAM_SRC,GstBTStreamSrcClass))
#define GST_IS_BTSTREAM_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_BTSTREAM_SRC))
#define GST_IS_BTSTREAM_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_BTSTREAM_SRC))

typedef struct _GstBTStreamSrc GstBTStreamSrc;
typedef struct _GstBTStreamSrcClass GstBTStreamSrcClass;

struct _GstBTStreamSrc {
	GstPushSrc element;

	btstream::BTStream* m_btstream;
	int piece_number;

	// Writable properties
	gchar* m_torrent;
	gchar* m_algorithm;
	int m_stream_length;
	gchar* m_save_path;
	gchar* m_seed_ip;
	unsigned short m_seed_port;
};

struct _GstBTStreamSrcClass {
	GstPushSrcClass parent_class;
};

GType gst_btstream_src_get_type(void);

G_END_DECLS

#endif /* __GST_BTSTREAM_SRC_H__ */
