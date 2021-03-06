#!/usr/bin/python
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
#  Created on: 23/01/2012
#      Author: gabriel
#

# Saving command line arguments so that GStreamer don't steal them.
import sys
argv = sys.argv
sys.argv = []

import argparse

import gobject
gobject.threads_init()

from videotorrentplayer import VideoTorrentPlayer
from messagehandler import MessageHandler

import logger

class Main(gobject.MainLoop):
    def __init__(self):
        super(Main, self).__init__()

        self.parse_args()

        logger.log_event("Download started.")

        self.pipeline = VideoTorrentPlayer(self.torrent_path, 
            self.use_fake_sink, self.save_path, self.algorithm,
            self.stream_length, self.buffer_size)

        self.message_handler = MessageHandler(self, self.pipeline)

    def parse_args(self):
        parser = argparse.ArgumentParser(description="BitTorrent video player.")

        parser.add_argument("torrent_path", help="Torrent file to open")
        parser.add_argument("-f", dest="fake_sink", action="store_true", 
            help="Produce fake audio/video output")
        parser.add_argument("--save_path", help="Where to save files", 
            default=".")
        parser.add_argument("--algorithm",
            choices=["rarest-first", "sequential", "deadline"],
            default="rarest-first", help="Piece picking algorithm")
        parser.add_argument("--stream_length", type=int, default=0,
            help="Length of the video in milliseconds")
        parser.add_argument("--buffer_size", type=float, default=4.0,
            help="Size of playback buffer in MB")

        args = parser.parse_args()
        
        self.torrent_path = args.torrent_path
        self.use_fake_sink = args.fake_sink
        self.save_path = args.save_path
        self.algorithm = args.algorithm
        self.stream_length = args.stream_length
        self.buffer_size = args.buffer_size

    def quit(self):
        logger.log_event("Stopping...")

        self.pipeline.close()
        
        super(Main, self).quit()

if __name__ == "__main__":
    # Loading saved command line arguments
    sys.argv = argv

    try:
        main = Main()
        main.run()
    except KeyboardInterrupt:
        main.quit()

