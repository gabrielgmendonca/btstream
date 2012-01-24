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

from time import time

import gst

import logger
import stats

class BufferManager:
    def __init__(self, pipeline):
        self.pipeline = pipeline
        self.buffering_time = []

        self.start_buffering()

    def update(self, percent):
        if percent == 100 and self.is_buffering:
            self.stop_buffering()

        elif not self.is_buffering:
            self.start_buffering()
            
    def start_buffering(self):
        self.pipeline.set_state(gst.STATE_PAUSED)

        self.is_buffering = True

        self.buffering_time.append(time())

        logger.log("Buffering.")

    def stop_buffering(self):
        self.pipeline.set_state(gst.STATE_PLAYING)

        self.is_buffering = False

        # Calculating duration of last playback interruption
        duration = time() - self.buffering_time[-1]
        self.buffering_time[-1] = duration

        logger.log("Starting playback.")

    def log(self):
        initial_wait = self.buffering_time[0]
        interruptions = len(self.buffering_time) - 1
        mean_time = stats.avg(self.buffering_time[1:])
        std_time = stats.std(self.buffering_time[1:])

        logger.log("--*--Buffer statistics--*--")
        logger.log("Time to start playback: %f" % initial_wait)
        logger.log("Number of interruptions: %d" % interruptions)
        logger.log("Interruption time - mean: %f" % mean_time)
        logger.log("Interruption time - standard deviation: %f" % std_time)
        logger.log("Interruptions: %r" % self.buffering_time)
        


