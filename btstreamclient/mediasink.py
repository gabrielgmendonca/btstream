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

class MediaSink(gst.Bin):
    def __init__(self, name):
        super(MediaSink, self).__init__(name)

        # Creating elements
        self.queue = gst.element_factory_make("queue", name + "-queue")
        
        # Adding elements
        self.add(self.queue)

        # Adding ghost pad
        sink_pad = gst.GhostPad("sink", self.queue.get_pad("sink"))
        self.add_pad(sink_pad)


