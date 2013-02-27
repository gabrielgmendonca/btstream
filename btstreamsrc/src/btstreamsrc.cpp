/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2011-2012 Gabriel Mendonça <gabrielgmendonca@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library Lesser General Public License for more details.
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

#include "propertyinstaller.h"

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
	PROP_ALGORITHM,
	PROP_STREAM_LENGTH,
	PROP_SAVE_PATH,
	PROP_SEED_IP,
	PROP_SEED_PORT,
	PROP_DOWNLOAD_RATE,
	PROP_UPLOAD_RATE,
	PROP_DOWNLOAD_PROGRESS,
	PROP_PIECES,
	PROP_NUM_PIECES,
	PROP_PEERS,
	PROP_SEEDS,
	PROP_CONNECTED_PEERS,
	PROP_CONNECTED_SEEDS,
	PROP_UPLOADS,
	PROP_DISTRIBUTED_COPIES,
	PROP_NEXT_ANNOUNCE
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

	std::string torrent_path, save_path, seed_ip;
	btstream::Algorithm algorithm = btstream::RAREST_FIRST;

	if (src->m_torrent) {
		torrent_path = src->m_torrent;
	}

	if (src->m_algorithm) {
		if (g_strcmp0(src->m_algorithm, "sequential") == 0) {
			algorithm = btstream::SEQUENTIAL;
		} else if (g_strcmp0(src->m_algorithm, "deadline") == 0) {
			algorithm = btstream::DEADLINE;
		}
	}

	int stream_length = src->m_stream_length;

	if (src->m_save_path) {
		save_path = src->m_save_path;
	}

	if (src->m_seed_ip) {
		seed_ip = src->m_seed_ip;
	}

	unsigned short seed_port = src->m_seed_port;

	src->m_btstream = new btstream::BTStream(torrent_path, algorithm,
			stream_length, save_path, seed_ip, seed_port);

	GST_INFO("Creating BTStreamSrc and starting torrent download.");

	return (src->m_btstream != 0);
}

static GstFlowReturn gst_btstream_src_create(GstPushSrc* psrc,
		GstBuffer** buffer) {
	GstBTStreamSrc* src;
	GstFlowReturn res;

	src = GST_BTSTREAM_SRC(psrc);

	GST_LOG("Requesting buffer from piece %d.", src->piece_number);

	boost::shared_ptr<btstream::Piece> piece;

	piece = src->m_btstream->get_next_piece();

	if (piece) {
		res = gst_pad_alloc_buffer(GST_BASE_SRC_PAD(psrc),
				GST_BUFFER_OFFSET_NONE, piece->size,
				GST_PAD_CAPS(GST_BASE_SRC_PAD(psrc)), buffer);

		if (res == GST_FLOW_OK) {
			unsigned char* data = GST_BUFFER_DATA(*buffer);

			mempcpy(data, piece->data.get(), piece->size);

			GST_LOG("Buffer from piece %d created.", src->piece_number++);

		} else {
			GST_WARNING("Flow error when creating buffer: %d.", res);
		}

	} else if (src->m_btstream->unlocked()) {
		// Access to resource (torrent pieces) was unlocked.
		res = GST_FLOW_WRONG_STATE;

		GST_INFO("Resources unlocked.");

	} else {
		// All pieces have already been returned.
		res = GST_FLOW_UNEXPECTED;

		GST_INFO("End of stream. All pieces returned.");
	}

	return res;
}

static gboolean gst_btstream_src_stop(GstBaseSrc * basesrc) {
	GstBTStreamSrc *src = GST_BTSTREAM_SRC(basesrc);

	if (src->m_btstream) {
		delete src->m_btstream;
	}

	GST_INFO("Closing source.");

	return TRUE;
}

static GstStateChangeReturn gst_btstream_src_change_state(GstElement *element,
		GstStateChange transition) {

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstBTStreamSrc* src = GST_BTSTREAM_SRC(element);

	switch (transition) {
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		src->m_btstream->notify_playback();
		GST_LOG("Starting playback. Updating deadlines.");
		break;
	default:
		break;
	}

	ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		return ret;
	}

	switch (transition) {
	case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
		src->m_btstream->notify_stall();
		GST_LOG("An interruption occurred.");
		break;
	default:
		break;
	}

	return ret;
}

static gboolean gst_btstream_src_unlock(GstBaseSrc *bsrc) {
	GstBTStreamSrc* src;

	src = GST_BTSTREAM_SRC(bsrc);
	src->m_btstream->unlock();

	GST_INFO("Unlock requested.");

	return TRUE;
}

static void gst_btstream_src_set_property(GObject * object, guint prop_id,
		const GValue * value, GParamSpec * pspec) {
	GstBTStreamSrc* src = GST_BTSTREAM_SRC (object);

	switch (prop_id) {
	case PROP_TORRENT:
		g_free(src->m_torrent);
		src->m_torrent = g_value_dup_string(value);
		break;

	case PROP_ALGORITHM:
		g_free(src->m_algorithm);
		src->m_algorithm = g_value_dup_string(value);
		break;

	case PROP_STREAM_LENGTH:
		src->m_stream_length = g_value_get_int(value);
		break;

	case PROP_SAVE_PATH:
		g_free(src->m_save_path);
		src->m_save_path = g_value_dup_string(value);
		break;

	case PROP_SEED_IP:
		g_free(src->m_seed_ip);
		src->m_seed_ip = g_value_dup_string(value);
		break;

	case PROP_SEED_PORT:
		src->m_seed_port = g_value_get_int(value);
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

	case PROP_ALGORITHM:
		g_value_set_string(value, src->m_algorithm);
		break;

	case PROP_STREAM_LENGTH:
		g_value_set_int(value, src->m_stream_length);
		break;

	case PROP_SAVE_PATH:
		g_value_set_string(value, src->m_save_path);
		break;

	case PROP_SEED_IP:
		g_value_set_string(value, src->m_seed_ip);
		break;

	case PROP_SEED_PORT:
		g_value_set_int(value, src->m_seed_port);
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
			g_value_set_float(value,
					src->m_btstream->get_status().download_progress);
		}
		break;

	case PROP_PIECES:
		if (src->m_btstream) {
			std::string pieces;
			boost::to_string(src->m_btstream->get_status().pieces, pieces);
			g_value_set_string(value, pieces.c_str());
		}
		break;

	case PROP_NUM_PIECES:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().num_pieces);
		}
		break;

	case PROP_PEERS:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().num_peers);
		}
		break;

	case PROP_SEEDS:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().num_seeds);
		}
		break;

	case PROP_CONNECTED_PEERS:
		if (src->m_btstream) {
			g_value_set_int(value,
					src->m_btstream->get_status().num_connected_peers);
		}
		break;

	case PROP_CONNECTED_SEEDS:
		if (src->m_btstream) {
			g_value_set_int(value,
					src->m_btstream->get_status().num_connected_seeds);
		}
		break;

	case PROP_UPLOADS:
		if (src->m_btstream) {
			g_value_set_int(value, src->m_btstream->get_status().num_uploads);
		}
		break;

	case PROP_DISTRIBUTED_COPIES:
		if (src->m_btstream) {
			g_value_set_int(value,
					src->m_btstream->get_status().distributed_copies);
		}
		break;

	case PROP_NEXT_ANNOUNCE:
		if (src->m_btstream) {
			g_value_set_int(value,
					src->m_btstream->get_status().seconds_to_next_announce);
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

	gst_element_class_set_details_simple(element_class, "BTStreamSrc", "source",
			"BitTorrent media source.",
			"Gabriel Mendonça <<gabrielgmendonca@poli.ufrj.br>>");

	gst_element_class_add_pad_template(element_class,
			gst_static_pad_template_get(&src_factory));
}

/* initialize the btstreamsrc's class */
static void gst_btstream_src_class_init(GstBTStreamSrcClass * klass) {
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseSrcClass *gstbasesrc_class;
	GstPushSrcClass *gstpushsrc_class;

	gobject_class = (GObjectClass*) (klass);
	gstelement_class = (GstElementClass*) (klass);
	gstbasesrc_class = (GstBaseSrcClass*) (klass);
	gstpushsrc_class = (GstPushSrcClass*) (klass);

	// Overrided methods
	gobject_class->set_property = gst_btstream_src_set_property;
	gobject_class->get_property = gst_btstream_src_get_property;

	gstelement_class->change_state = gst_btstream_src_change_state;

	gstbasesrc_class->start = gst_btstream_src_start;
	gstbasesrc_class->stop = gst_btstream_src_stop;
	gstbasesrc_class->unlock = gst_btstream_src_unlock;
	gstpushsrc_class->create = gst_btstream_src_create;

	// Properties
	PropertyInstaller installer(gobject_class);

	// Read-write properties
	installer.install_string(PROP_TORRENT, "torrent", "Torrent",
			"Torrent file path.", "", true);
	installer.install_string(PROP_ALGORITHM, "algorithm", "Algorithm",
			"Piece picking algorithm: rarest-first, sequential or deadline.",
			"rarest-first", true);
	installer.install_int(PROP_STREAM_LENGTH, "stream_length", "Stream Length",
			"Estimation of decoded stream's length in milliseconds. Used by deadline algorithm.",
			0, 999999999, 0, true);
	installer.install_string(PROP_SAVE_PATH, "save_path", "Save Path",
			"Where to save downloaded files.", "./", true);
	installer.install_string(PROP_SEED_IP, "seed_ip", "Seed IP",
			"IP of a known seed.", "", true);
	installer.install_int(PROP_SEED_PORT, "seed_port", "Seed Port",
			"Port of a known seed.", 0, 65535, 0, true);

	// Read-only properties
	installer.install_int(PROP_DOWNLOAD_RATE, "download_rate", "Download Rate",
			"Torrent download rate in B/s.", 0, 999999999, 0);
	installer.install_int(PROP_UPLOAD_RATE, "upload_rate", "Upload Rate",
			"Torrent upload rate in B/s.", 0, 999999999, 0);
	installer.install_float(PROP_DOWNLOAD_PROGRESS, "download_progress",
			"Download Progress", "Torrent download progress.");
	installer.install_string(PROP_PIECES, "pieces", "Pieces",
			"Binary representation of received pieces.");
	installer.install_int(PROP_NUM_PIECES, "num_pieces", "Number of Pieces",
			"Number of downloaded pieces.", 0, 999999999, 0);
	installer.install_int(PROP_PEERS, "num_peers", "Number of Peers",
			"Number of known peers in the swarm.", 0, 999999999, 0);
	installer.install_int(PROP_SEEDS, "num_seeds", "Number of Seeds",
			"Number of known seeds in the swarm.", 0, 999999999, 0);
	installer.install_int(PROP_CONNECTED_PEERS, "num_connected_peers",
			"Number of Connected Peers", "Number of connected peers.", 0,
			999999999, 0);
	installer.install_int(PROP_CONNECTED_SEEDS, "num_connected_seeds",
			"Number of Connected Seeds", "Number of connected seeds.", 0,
			999999999, 0);
	installer.install_int(PROP_UPLOADS, "num_uploads", "Number of Uploads",
			"Number of unchocked peers.", 0, 999999999, 0);
	installer.install_int(PROP_DISTRIBUTED_COPIES, "distributed_copies",
			"Number of Distributed Copies",
			"Number of copies of the rarest piece(s) among connected peers.", 0,
			999999999, 0);
	installer.install_int(PROP_NEXT_ANNOUNCE, "next_announce", "Next Announce",
			"Seconds until next announce to tracker.", 0, 999999999, 0);
}

/*
 * initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 */
static void gst_btstream_src_init(GstBTStreamSrc * src,
		GstBTStreamSrcClass * gclass) {
}

/*
 * entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean btstreamsrc_init(GstPlugin * btstreamsrc) {
	/* debug category for filtering log messages
	 */
	GST_DEBUG_CATEGORY_INIT(gst_btstream_src_debug, "btstreamsrc", 0,
			"BitTorrent media source");

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

/*
 * gstreamer looks for this structure to register btstreamsrcs
 */
GST_PLUGIN_DEFINE( GST_VERSION_MAJOR, GST_VERSION_MINOR, "btstreamsrc",
		"BitTorrent media source", btstreamsrc_init, VERSION, "LGPL",
		"GStreamer", "http://code.google.com/p/btstream/");
