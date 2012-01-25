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

import gst

import logger

class MessageHandler:
    def __init__(self, main_loop, pipeline):
        self.main_loop = main_loop
        self.pipeline = pipeline

        self.bus = self.pipeline.get_bus()
        self.bus.add_signal_watch()
        self.bus.connect("message::eos", self.handle_eos_message)
        self.bus.connect("message::buffering", self.handle_buffering_message)
        self.bus.connect("message::error", self.handle_error_message)

    def handle_eos_message(self, bus, message):
        logger.log_event("Playback finished.")
        self.exit()

    def handle_buffering_message(self, bus, message):
        percent = message.parse_buffering()
        self.pipeline.buffer_manager.update(percent)

    def handle_error_message(self, bus, message):
        err, debug = message.parse_error()
        logger.log_error(err, debug)
        self.exit()

    def exit(self):
        logger.log_event("Stopping...")
        self.pipeline.set_state(gst.STATE_NULL)
        self.pipeline.log()
        
        self.main_loop.quit()



