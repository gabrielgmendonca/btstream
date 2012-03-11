/*
 * Copyright (C) 2011-2012 Gabriel Mendonça
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

#include "VideoTorrentPlugin.h"
#include "VideoPeerPlugin.h"

namespace btstream {

VideoTorrentPlugin::VideoTorrentPlugin(torrent* t, PiecePicker* pp) :
		m_torrent(t), m_piece_picker(pp) {

	if (m_piece_picker) {
		m_piece_picker->init(m_torrent);
	}
}

void VideoTorrentPlugin::on_piece_pass(int index) {
	m_torrent->read_piece(index);

	if (m_piece_picker) {
		m_piece_picker->add_piece_request(m_torrent);
	}
}

boost::shared_ptr<torrent_plugin> create_video_plugin(torrent* t, void* params) {
	return boost::shared_ptr<torrent_plugin>(new VideoTorrentPlugin(t, (PiecePicker*) params));
}

} /* namespace btstream */
