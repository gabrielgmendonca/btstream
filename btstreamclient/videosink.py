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

from mediasink import MediaSink

class VideoSink(MediaSink):
    def __init__(self):
        super(VideoSink, self).__init__("video-sink")

        # Creating elements
        self.converter = gst.element_factory_make("ffmpegcolorspace", "video-converter")
        self.sink = gst.element_factory_make("autovideosink", "auto-video-sink")

        # Adding elements
        self.add(self.converter, self.sink)

        # Linking elements (queue from MediaSink)
        gst.element_link_many(self.queue, self.converter, self.sink)



