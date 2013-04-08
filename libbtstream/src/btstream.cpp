/*
 * Copyright (C) 2011-2013 Gabriel Mendonça
 *
 * This file is part of BTStream.
 * BTStream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BTStream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * BTStream.cpp
 *
 *  Created on: 04/12/2011
 *      Author: gabriel
 */

#include "btstream.h"

namespace btstream {

BTStream::BTStream(const std::string& torrent_path,
		const std::string& save_path, Algorithm algorithm, int stream_length) :
		m_video_torrent_manager(new VideoTorrentManager) {

	int num_pieces;
	num_pieces = m_video_torrent_manager->add_torrent(torrent_path, save_path,
			algorithm, stream_length);

	init(num_pieces);
}

BTStream::BTStream(const std::string& torrent_path, PiecePicker* piece_picker,
		const std::string& save_path) :
		m_video_torrent_manager(new VideoTorrentManager) {

	int num_pieces;
	num_pieces = m_video_torrent_manager->add_torrent(torrent_path, piece_picker,
			save_path);

	init(num_pieces);
}

boost::shared_ptr<Piece> BTStream::get_next_piece() {
	return m_video_buffer->get_next_piece();
}

Status BTStream::get_status() {
	return m_video_torrent_manager->get_status();
}

void BTStream::notify_playback() {
	m_video_torrent_manager->notify_playback();
}

void BTStream::notify_stall() {
	m_video_torrent_manager->notify_stall();
}

void BTStream::unlock() {
	m_video_buffer->unlock();
}

bool BTStream::unlocked() {
	return m_video_buffer->unlocked();
}

void BTStream::init(int num_pieces) {
	m_video_buffer = boost::shared_ptr<VideoBuffer>(
			new VideoBuffer(num_pieces));

	m_video_torrent_manager->start_download(m_video_buffer);
}

} /* namespace btstream */
