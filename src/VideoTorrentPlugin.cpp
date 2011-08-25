/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BIVoD.
 * BIVoD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BIVoD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BIVoD.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * VideoTorrentPlugin.cpp
 *
 *  Created on: 24/08/2011
 *      Author: gabriel
 */

#include <iostream>
#include "VideoTorrentPlugin.h"

namespace bivod {

VideoTorrentPlugin::VideoTorrentPlugin() :
		paused(true) {
}

void VideoTorrentPlugin::on_piece_pass(int index) {
	std::cout << index << std::endl;

	if (index < playback_position + BUFFER_SIZE) {
		buffer_count++;

		if (buffer_count == BUFFER_SIZE) {
			start_playback();
		}
	}
}

void VideoTorrentPlugin::start_playback() {
	paused = false;
	buffer_count = 0;

	std::cout << "Playing!" << std::endl;
}

boost::shared_ptr<torrent_plugin> create_video_plugin(torrent* t, void*) {
	return boost::shared_ptr<torrent_plugin>(new VideoTorrentPlugin());
}

} /* namespace bivod */
