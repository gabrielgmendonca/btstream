/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BTStream.
 * BTStream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BTStream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * VideoTorrentPlugin.cpp
 *
 *  Created on: 24/08/2011
 *      Author: gabriel
 */

#include <iostream>

#include "VideoTorrentPlugin.h"
#include "VideoPeerPlugin.h"

namespace btstream {

VideoTorrentPlugin::VideoTorrentPlugin(torrent* t) :
		m_torrent(t) {}

boost::shared_ptr<peer_plugin> VideoTorrentPlugin::new_connection(peer_connection* pc) {
	return boost::shared_ptr<peer_plugin>(new VideoPeerPlugin(pc));
}

void VideoTorrentPlugin::on_piece_pass(int index) {
	m_torrent->read_piece(index);
}

boost::shared_ptr<torrent_plugin> create_video_plugin(torrent* t, void* params) {
	return boost::shared_ptr<torrent_plugin>(new VideoTorrentPlugin(t));
}

} /* namespace btstream */
