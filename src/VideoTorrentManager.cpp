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

VideoTorrentManager::VideoTorrentManager(VideoPlayer* video_player) :
		m_video_player(video_player) {

	TorrentPluginFactory f(&create_video_plugin);
	m_session.add_extension(f);

	// Sets alert mask in order to receive downloaded pieces as alerts.
	m_session.set_alert_mask(alert::storage_notification);
}

void VideoTorrentManager::add_torrent(std::string file_name,
		std::string save_path) {

	add_torrent_params params;
	params.ti = new torrent_info((const boost::filesystem::path) file_name);
	params.save_path = save_path;

	// Starts torrent.
	m_torrent_handle = m_session.add_torrent(params);

	// Starts VideoPlayer feeding thread that calls the feed_video_player method.
	boost::thread feeding_thread(&VideoTorrentManager::feed_video_player, this);

	// Gets the number of pieces that will be downloaded and played.
	m_pieces_to_play = params.ti.get()->num_pieces();
}

void VideoTorrentManager::feed_video_player() {
	while (m_pieces_to_play > 0) {
		std::auto_ptr<alert> a = m_session.pop_alert();

		if (a.get()) {
			// Casts alert pointer to read_piece_alert pointer.
			std::auto_ptr<read_piece_alert> piece_alert(alert_cast<read_piece_alert>(a.release()));

			if (piece_alert.get()) {
				// Sends piece to VideoPlayer.
				int index = piece_alert->piece;
				boost::shared_array<char> data = piece_alert->buffer;
				int size = piece_alert->size;

				m_video_player->add_piece(index, data, size);
			}

			m_pieces_to_play--;
		}

		// Sleeps thread for 250 milliseconds.
		boost::this_thread::sleep(boost::posix_time::milliseconds(250));
	}
}

} /* namespace bivod */
