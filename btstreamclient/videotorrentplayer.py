# -*- coding: utf8 -*-
#
# Copyright (C) 2012 Gabriel Mendonça
#
# This file is part of BTStream.
# BTStream is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# BTStream is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  Created on: 24/01/2012
#      Author: gabriel
#

import gst

from audiosink import AudioSink
from videosink import VideoSink
from fakesink import FakeSink

class VideoTorrentPlayer(gst.Pipeline):
    """
    Provides a GStreamer pipeline for video playing from a torrent
    source.
    """

    def __init__(self, torrent_path, use_fake_sink=False):
        super(VideoTorrentPlayer, self).__init__("video-torrent-player")

        self.torrent_path = torrent_path
        self.use_fake_sink = use_fake_sink

        # Creating elements
        self.src = gst.element_factory_make("btstreamsrc", "src")
        self.decoder = gst.element_factory_make("decodebin2", "decoder")
        # Creating sink elements (audio, video)
        self.create_sinks()

        # Configuring elements
        self.src.set_property("torrent", self.torrent_path)
        self.decoder.set_property("use-buffering", True)
        self.decoder.set_property("low-percent", 5)

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


