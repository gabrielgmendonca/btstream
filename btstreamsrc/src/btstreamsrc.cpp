/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2011-2012 Gabriel Mendonça <gabrielgmendonca@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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

/**
 * SECTION:element-btstreamsrc
 *
 * Read from a BitTorrent swarm
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v btstreamsrc ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "btstreamsrc.h"

#include <string.h>
#include <time.h>

GST_DEBUG_CATEGORY_STATIC(gst_btstream_src_debug);
#define GST_CAT_DEFAULT gst_btstream_src_debug

/* Filter signals and args */
enum {
	/* FILL ME */
	LAST_SIGNAL
};

enum {
	PROP_0,
	PROP_TORRENT,
	PROP_DOWNLOAD_RATE,
	PROP_UPLOAD_RATE,
	PROP_DOWNLOAD_PROGRESS,
	PROP_PIECES
};

GST_BOILERPLATE(GstBTStreamSrc, gst_btstream_src, GstPushSrc,
		GST_TYPE_PUSH_SRC);

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("ANY")
);

static gboolean gst_btstream_src_start(GstBaseSrc * basesrc) {
	GstBTStreamSrc *src = GST_BTSTREAM_SRC(basesrc);

	std::string path = src->m_torrent;
	src->m_btstream = new btstream::BTStream(path);

	return (src->m_btstream != 0);
}

int pnum = 0;

static GstFlowReturn gst_btstream_src_create(GstPushSrc* psrc,
		GstBuffer** buffer) {
	GstBTStreamSrc* src;
	GstFlowReturn res;

	src = GST_BTSTREAM_SRC(psrc);

	boost::shared_ptr < btstream::Piece > piece;

	g_print("[SRC-DEBUG] Asking for piece buffer: %d\r\n", pnum);
	piece = src->m_btstream->get_next_piece();
	g_print("[SRC-DEBUG] Piece buffer received: %d\r\n", pnum++);

	if (piece) {
		res = gst_pad_alloc_buffer(GST_BASE_SRC_PAD(psrc),
				GST_BUFFER_OFFSET_NONE, piece->size,
				GST_PAD_CAPS(GST_BASE_SRC_PAD(psrc)), buffer);

		if (res == GST_FLOW_OK) {
			unsigned char* data = GST_BUFFER_DATA(*buffer);

			mempcpy(data, piece->data.get(), piece->size);

		} else {
			g_print("[SRC-DEBUG] Flow Error: %d!\r\n", res);
		}

	} else if (src->m_btstream->unlocked()) {
		// Access to resource (torrent pieces) was unlocked.
		res = GST_FLOW_WRONG_STATE;

	} else {
		// All pieces have already been returned.
		res = GST_FLOW_UNEXPECTED;
	}

	return res;
}

static gboolean gst_btstream_src_stop(GstBaseSrc * basesrc) {
	GstBTStreamSrc *src = GST_BTSTREAM_SRC(basesrc);

	if (src->m_btstream) {
		delete src->m_btstream;
	}

	return TRUE;
}

static gboolean gst_btstream_src_unlock(GstBaseSrc *bsrc) {
	GstBTStreamSrc* src;

	src = GST_BTSTREAM_SRC(bsrc);
	src->m_btstream->unlock();

	return TRUE;
}

static void install_properties(GObjectClass *gobject_class)
{
	GParamSpec* pspec;
	GParamFlags pflags;

	// Properties
    pflags = static_cast<GParamFlags>(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    pspec = g_param_spec_string("torrent", "Torrent", "Torrent file path", "", pflags);
    g_object_class_install_property(gobject_class, PROP_TORRENT, pspec);

    pflags = static_cast<GParamFlags>(G_PARAM_READABLE);
    pspec = g_param_spec_int("download_rate", "Download Rate",
    		"Torrent download rate in KiB/s", 0, 999999999, 0, pflags);
    g_object_class_install_property(gobject_class, PROP_DOWNLOAD_RATE, pspec);

    pflags = static_cast<GParamFlags>(G_PARAM_READABLE);
    pspec = g_param_spec_int("upload_rate", "Upload Rate",
    		"Torrent upload rate in KiB/s", 0, 999999999, 0, pflags);
    g_object_class_install_property(gobject_class, PROP_UPLOAD_RATE, pspec);

    pflags = static_cast<GParamFlags>(G_PARAM_READABLE);
    pspec = g_param_spec_float("download_progress", "Download Progress",
    		"Torrent download progress", 0.0f, 1.0f, 0.0f, pflags);
    g_object_class_install_property(gobject_class, PROP_DOWNLOAD_PROGRESS, pspec);

    pflags = static_cast<GParamFlags>(G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
    pspec = g_param_spec_string("pieces", "Pieces", "Binary representation of received pieces",
    		"", pflags);
    g_object_class_install_property(gobject_class, PROP_PIECES, pspec);
}

static void gst_btstream_src_set_property(GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec) {
	GstBTStreamSrc* src = GST_BTSTREAM_SRC (object);

	switch (prop_id) {
	case PROP_TORRENT:
		g_free(src->m_torrent);
		src->m_torrent = g_value_dup_string(value);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gst_btstream_src_get_property(GObject * object, guint prop_id,
		GValue * value, GParamSpec * pspec) {
	GstBTStreamSrc* src = GST_BTSTREAM_SRC (object);

	switch (prop_id) {
	case PROP_TORRENT:
		g_value_set_string(value, src->m_torrent);
		break;

	case PROP_DOWNLOAD_RATE:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().download_rate);
		}
		break;

	case PROP_UPLOAD_RATE:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().upload_rate);
		}
		break;

	case PROP_DOWNLOAD_PROGRESS:
		if (src->m_btstream) {
			g_value_set_float(value, src->m_btstream->get_status().download_progress);
		}
		break;

	case PROP_PIECES:
		if (src->m_btstream) {
			std::string pieces;
			boost::to_string(src->m_btstream->get_status().pieces, pieces);
			g_value_set_string(value, pieces.c_str());
		}
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

/* GstElement vmethod implementations */

/* GObject vmethod implementations */
static void gst_btstream_src_base_init(gpointer gclass) {
	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_set_details_simple(element_class, "BTStreamSrc",
			"FIXME:Generic", "FIXME:Generic Template Element",
			"Gabriel Mendonça <<gabrielgmendonca@poli.ufrj.br>>");

	gst_element_class_add_pad_template(element_class,
			gst_static_pad_template_get(&src_factory));
}

/* initialize the btstreamsrc's class */
static void gst_btstream_src_class_init(GstBTStreamSrcClass * klass) {
	GObjectClass *gobject_class;
	GstBaseSrcClass *gstbasesrc_class;
	GstPushSrcClass *gstpushsrc_class;

	gobject_class = (GObjectClass *)(klass);
    gstbasesrc_class = (GstBaseSrcClass*)(klass);
    gstpushsrc_class = (GstPushSrcClass*)(klass);

    // Overrided methods
    // FIXME: Need to override other methods?
    gobject_class->set_property = gst_btstream_src_set_property;
    gobject_class->get_property = gst_btstream_src_get_property;

    gstbasesrc_class->start = gst_btstream_src_start;
    gstbasesrc_class->stop = gst_btstream_src_stop;
    gstbasesrc_class->unlock = gst_btstream_src_unlock;
    gstpushsrc_class->create = gst_btstream_src_create;

    // Properties
    install_properties(gobject_class);
}

/*
 * initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_btstream_src_init(GstBTStreamSrc * src,
		GstBTStreamSrcClass * gclass) {

	// FIXME: Add Pads?
//	src->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
//	gst_pad_set_getcaps_function(src->srcpad,
//			GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
//
//	gst_element_add_pad(GST_ELEMENT (src), src->srcpad);
}

/*
 * entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean btstreamsrc_init(GstPlugin * btstreamsrc) {
	/* debug category for fltering log messages
	 *
	 * exchange the string 'Template btstreamsrc' with your description
	 */
	GST_DEBUG_CATEGORY_INIT(gst_btstream_src_debug, "btstreamsrc", 0,
			"Template btstreamsrc");

	return gst_element_register(btstreamsrc, "btstreamsrc", GST_RANK_NONE,
			GST_TYPE_BTSTREAM_SRC);
}

/*
 * PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "btstream"
#endif

#define VERSION "0.2"

/* gstreamer looks for this structure to register btstreamsrcs
 *
 * exchange the string 'Template btstreamsrc' with your btstreamsrc description
 */
GST_PLUGIN_DEFINE( GST_VERSION_MAJOR, GST_VERSION_MINOR, "btstreamsrc",
		"BitTorrent media source", btstreamsrc_init, VERSION, "LGPL", "GStreamer",
		"http://code.google.com/p/btstream/");
