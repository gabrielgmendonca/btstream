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

from videotorrentplayer import VideoTorrentPlayer
from messagehandler import MessageHandler

import logger

class Main(gobject.MainLoop):
    def __init__(self, args):
        super(Main, self).__init__()

        self.parse_args(args)

        logger.log_event("Download started.")

        self.pipeline = VideoTorrentPlayer(self.torrent_path, self.use_fake_sink)
        self.message_handler = MessageHandler(self, self.pipeline)

    def parse_args(self, args):
        if len(args) == 1:
            self.torrent_path = args[0]
            self.use_fake_sink = False

        elif len(args) == 2 and args[0] == "-f":
            self.torrent_path = args[1]
            self.use_fake_sink = True
            
        else:
            print "Usage: python btstreamclient.py [-f] torrent_path"
            sys.exit()

    def quit(self):
        logger.log_event("Stopping...")

        self.pipeline.close()
        
        super(Main, self).quit()

if __name__ == "__main__":
    try:
        main = Main(sys.argv[1:])
        main.run()
    except KeyboardInterrupt:
        main.quit()

