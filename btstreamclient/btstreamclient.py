#!/usr/bin/python
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
#  Created on: 23/01/2012
#      Author: gabriel
#

import sys

import gobject
gobject.threads_init()

import pygst
pygst.require("0.10")

import gst
import gtk

from videotorrentplayer import VideoTorrentPlayer
from messagehandler import MessageHandler

class Main:
    def __init__(self, args):
        if len(args) != 1:
            print "Usage: python btstreamclient.py torrent_path"
            sys.exit()

        torrent_path = args[0]
        self.pipeline = VideoTorrentPlayer(torrent_path)

        self.message_handler = MessageHandler(self.pipeline)

        # Starting playback
        print "Starting download..."
        self.pipeline.set_state(gst.STATE_PAUSED)
        #self.audio_sink.set_state(gst.STATE_PAUSED)
        #self.video_sink.set_state(gst.STATE_PAUSED)



if __name__ == "__main__":
    start=Main(sys.argv[1:])
    gtk.main()

