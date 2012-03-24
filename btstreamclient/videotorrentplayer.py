# -*- coding: utf8 -*-
#
# Copyright (C) 2012 Gabriel Mendonça
#
# This file is part of BTStream.
# BTStream is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BTStream is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  Created on: 24/01/2012
#      Author: gabriel
#

from time import time

import gobject

import gst

from audiosink import AudioSink
from videosink import VideoSink
from fakesink import FakeSink
from buffermanager import BufferManager

import logger
import stats

class VideoTorrentPlayer(gst.Pipeline):
    """
    Provides a GStreamer pipeline for video playing from a torrent
    source.
    """

    def __init__(self, torrent_path, use_fake_sink, algorithm, stream_length,
            buffer_size, seed_ip, seed_port):

        super(VideoTorrentPlayer, self).__init__("video-torrent-player")

        self.torrent_path = torrent_path
        self.use_fake_sink = use_fake_sink
        self.algorithm = algorithm
        self.stream_length = stream_length
        self.buffer_size = buffer_size
        self.seed_ip = seed_ip
        self.seed_port = seed_port
        
        self.download_rates = []
        self.upload_rates = []
        self.download_time = time()
        self.download_finished = False

        # Creating, configuring and linking GStreamer elements
        self.init_pipeline()

        # Creating BufferManager
        self.buffer_manager = BufferManager(self)

        # Adding torrent status checking thread
        gobject.timeout_add(1000, self.check_status)

    def init_pipeline(self):
        # Creating elements
        self.src = gst.element_factory_make("btstreamsrc", "src")
        self.decoder = gst.element_factory_make("decodebin2", "decoder")
        # Creating sink elements (audio, video)
        self.create_sinks()

        # Configuring elements
        self.src.set_property("torrent", self.torrent_path)
        self.src.set_property("algorithm", self.algorithm)
        self.src.set_property("stream_length", self.stream_length)

        if self.seed_ip is not None and self.seed_port is not None:
            self.src.set_property("seed_ip", self.seed_ip)
            self.src.set_property("seed_port", int(self.seed_port))

        # Configuring playback buffer        
        self.decoder.set_property("use-buffering", True)
        self.decoder.set_property("low-percent", 5)
        self.decoder.set_property("max-size-bytes", 
            int(self.buffer_size * 1024 * 1024))

        # Configuring callbacks
        self.decoder.connect("new-decoded-pad", self.handle_decoded_pad)

        # Adding elements
        self.add(self.src, self.decoder, self.audio_sink, self.video_sink)

        # Linking elements
        gst.element_link_many(self.src, self.decoder)

    def handle_decoded_pad(self, demuxer, new_pad, is_last):
        structure_name = new_pad.get_caps()[0].get_name()
        if structure_name.startswith("audio"):
            new_pad.link(self.audio_sink.get_pad("sink"))

        elif structure_name.startswith("video"):
            new_pad.link(self.video_sink.get_pad("sink"))

    def create_sinks(self):
        if not self.use_fake_sink:
            self.audio_sink = AudioSink()
            self.video_sink = VideoSink()

        else:
            self.audio_sink = FakeSink("audio-sink")
            self.video_sink = FakeSink("video-sink")

    def check_status(self):
        download_progress = self.src.get_property("download_progress") * 100

        if download_progress < 100:
            download_rate = self.src.get_property("download_rate") / 1024
            self.download_rates.append(download_rate)

            pieces = self.src.get_property("pieces")
            num_pieces = self.src.get_property("num_pieces")

            logger.log("Download progress: %d%%" % download_progress)
            logger.log("Download rate: %d KiB/s" % download_rate)
            logger.log("Pieces: %s" % pieces)
            logger.log("Number of pieces: %s" % num_pieces)

        elif not self.download_finished:
            self.download_finished = True
            self.download_time = time() - self.download_time

            logger.log_event("Download finished.")

        upload_rate = self.src.get_property("upload_rate") / 1024
        self.upload_rates.append(upload_rate)

        logger.log("Upload rate: %d KiB/s" % upload_rate)

        peers = self.src.get_property("num_peers")
        seeds = self.src.get_property("num_seeds")
        connected_peers = self.src.get_property("num_connected_peers")
        connected_seeds = self.src.get_property("num_connected_seeds")
        num_uploads = self.src.get_property("num_uploads")
        distributed_copies = self.src.get_property("distributed_copies")
        next_announce = self.src.get_property("next_announce")

        logger.log("Connected peers (total): %d (%d)" % (connected_peers, peers))
        logger.log("Connected seeds (total): %d (%d)" % (connected_seeds, seeds))
        logger.log("Number of unchocked peers: %d" % num_uploads)
        logger.log("Distributed copies: %d" % distributed_copies)
        logger.log("Next annouce to tracker (s): %d" % next_announce)

        return True

    def close(self):
        self.set_state(gst.STATE_NULL)
        self.log()
        logger.close()

    def log(self):
        mean_download_rate = stats.avg(self.download_rates)
        std_download_rate = stats.std(self.download_rates)

        mean_upload_rate = stats.avg(self.upload_rates)
        std_upload_rate = stats.std(self.upload_rates)

        logger.log("--*--Torrent statistics--*--")
        logger.log("Download rate (KiB/s) - mean: %f" % mean_download_rate)
        logger.log("Download rate (KiB/s) - standard deviation: %f" % std_download_rate)
        logger.log("Upload rate (KiB/s) - mean: %f" % mean_upload_rate)
        logger.log("Upload rate (KiB/s) - standard deviation: %f" % std_upload_rate)

        logger.log_to_file("download_rate_mean, %f\r\n" % mean_download_rate)
        logger.log_to_file("download_rate_stdev, %f\r\n" % std_download_rate)
        logger.log_to_file("upload_rate_mean, %f\r\n" % mean_upload_rate)
        logger.log_to_file("upload_rate_stdev, %f\r\n" % std_upload_rate)

        if self.download_finished:
            logger.log("Download time (s): %d" % self.download_time)
            logger.log_to_file("download_time, %d\r\n" % self.download_time)
        else:
            logger.log_to_file("download_time, %d\r\n" % -1)

        self.buffer_manager.log()


