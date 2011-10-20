/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BiVoD.
 * BiVoD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BiVoD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BiVoD.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * VideoTorrentManager.cpp
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#include "VideoTorrentManager.h"

#include <libtorrent/alert_types.hpp>

#include "VideoTorrentPlugin.h"

namespace bivod {

VideoTorrentManager::VideoTorrentManager() {
	TorrentPluginFactory f(&create_video_plugin);
	m_session.add_extension(f);

	// Sets alert mask in order to receive downloaded pieces as alerts.
	m_session.set_alert_mask(alert::storage_notification);
}

int VideoTorrentManager::add_torrent(std::string file_name,
		std::string save_path) {

	add_torrent_params params;
	params.ti = new torrent_info((const boost::filesystem::path) file_name);
	params.save_path = save_path;
	params.paused = true;
	params.auto_managed = false;

	m_torrent_handle = m_session.add_torrent(params);

	// Gets the number of pieces that will be downloaded and played.
	m_pieces_to_play = params.ti.get()->num_pieces();

	return m_pieces_to_play;
}

void VideoTorrentManager::start_download(VideoBuffer* video_buffer) {
	m_video_buffer = video_buffer;

	// Starts torrent download.
	m_torrent_handle.auto_managed(true);

	// Starts VideoBuffer feeding thread that calls the feed_video_buffer method.
	boost::thread feeding_thread(&VideoTorrentManager::feed_video_buffer, this);
}

void VideoTorrentManager::feed_video_buffer() {
	while (m_pieces_to_play > 0) {

		// Tries to get an alert from alert queue.
		time_duration max_wait_time(boost::posix_time::seconds(30).ticks());
		const alert* new_alert = m_session.wait_for_alert(max_wait_time);

		if (new_alert) {
			// Tries to cast alert pointer to read_piece_alert pointer.
			const read_piece_alert* piece_alert = alert_cast<read_piece_alert>(new_alert);

			if (piece_alert) {
				// Adds piece to VideoBuffer.
				int index = piece_alert->piece;
				boost::shared_array<char> data = piece_alert->buffer;
				int size = piece_alert->size;

				m_video_buffer->add_piece(index, data, size);
			}

			m_pieces_to_play--;

			// Removes alert from queue.
			m_session.pop_alert();
		}
	}
}

} /* namespace bivod */
