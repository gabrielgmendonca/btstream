/*
 * Copyright (C) 2011-2012 Gabriel Mendonça
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
 * VideoTorrentManager.cpp
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#include "videotorrentmanager.h"

#include <fstream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/peer_info.hpp>

#include "videotorrentplugin.h"

namespace btstream {

VideoTorrentManager::VideoTorrentManager() :
		m_last_played_piece(0), m_deadlines_mode(false) {

	TorrentPluginFactory f(&create_video_plugin);
	m_session.add_extension(f);

	// Defines port range to listen.
	m_session.listen_on(std::make_pair(6881, 6889));

	// Sets alert mask in order to receive downloaded pieces as alerts.
	m_session.set_alert_mask(
			alert::storage_notification | alert::progress_notification);

	// Starts extensions.
	m_session.start_dht();
}

VideoTorrentManager::~VideoTorrentManager() {
	if (m_feeding_thread) {
		// Stops feeding thread.
		m_feeding_thread->interrupt();
		m_feeding_thread->join();
	}
}

int VideoTorrentManager::add_torrent(std::string file_name, Algorithm algorithm,
		int stream_length, std::string save_path, std::string seed_ip,
		unsigned short seed_port) throw (Exception) {

	add_torrent(file_name, 0, save_path, seed_ip, seed_port);

	// Sets piece picking algorithm
	switch (algorithm) {
	case RAREST_FIRST:
		m_torrent_handle.set_sequential_download(false);
		break;

	case SEQUENTIAL:
		m_torrent_handle.set_sequential_download(true);
		break;

	case DEADLINE:
		if (stream_length == 0) {
			throw Exception("The decoded stream length must be provided.");
		}

		m_deadlines_mode = true;

		// Estimates decoded piece (audio/video) length.
		m_decoded_piece_length = (float) stream_length / m_num_pieces;

		// Starts on sequential mode.
		m_torrent_handle.set_sequential_download(true);

		break;
	}

	return m_num_pieces;
}

int VideoTorrentManager::add_torrent(std::string file_name,
		PiecePicker* piece_picker, std::string save_path, std::string seed_ip,
		unsigned short seed_port) throw (Exception) {

	int num_pieces = 0;
	try {
		add_torrent_params params;
		params.ti = read_torrent_file(file_name);
		params.save_path = save_path;
		params.paused = true;
		params.auto_managed = false;
		params.userdata = dynamic_cast<void*>(piece_picker);

		m_torrent_handle = m_session.add_torrent(params);

		num_pieces = params.ti.get()->num_pieces();

		// Adds seed
		if (seed_ip != "" && seed_port > 0) {
			tcp::endpoint seed_endpoint;
			seed_endpoint.address(address::from_string(seed_ip));
			seed_endpoint.port(seed_port);

			// Adds as peer
			m_torrent_handle.connect_peer(seed_endpoint, peer_info::seed);

			// Adds as DHT node
			std::pair<std::string, int> node(seed_ip, seed_port);
			m_session.add_dht_node(node);
		}

		m_next_piece = 0;
		m_num_pieces = num_pieces;

	} catch (std::exception& e) {
		throw Exception(e.what());
	}

	return num_pieces;
}

void VideoTorrentManager::start_download(
		boost::shared_ptr<VideoBuffer> video_buffer) throw (Exception) {
	m_video_buffer = video_buffer;

	// Starts torrent download.
	m_torrent_handle.resume();

	// Starts VideoBuffer feeding thread that calls the feed_video_buffer method.
	m_feeding_thread = boost::shared_ptr<boost::thread>(
			new boost::thread(&VideoTorrentManager::feed_video_buffer, this));
}

void VideoTorrentManager::feed_video_buffer() {
	try {
		while (m_next_piece < m_num_pieces) {

			// Tries to get an alert from alert queue.
			time_duration max_wait_time(boost::posix_time::seconds(30).ticks());
			const alert* new_alert = m_session.wait_for_alert(max_wait_time);

			if (new_alert) {
				// Tries to cast alert pointer to different alert types.
				const piece_finished_alert* finished_alert = alert_cast<
						piece_finished_alert>(new_alert);
				const read_piece_alert* read_alert =
						alert_cast<read_piece_alert>(new_alert);

				if (finished_alert) {
					if (finished_alert->piece_index == m_next_piece) {
						m_torrent_handle.read_piece(m_next_piece);
					}

				} else if (read_alert) {
					if (read_alert->piece == m_next_piece) {
						// Adds piece to VideoBuffer.
						int index = read_alert->piece;
						boost::shared_array<char> data = read_alert->buffer;
						int size = read_alert->size;

						m_video_buffer->add_piece(index, data, size);
						m_next_piece++;

						bool have_next =
								m_torrent_handle.status().pieces[m_next_piece];
						if (have_next) {
							m_torrent_handle.read_piece(m_next_piece);
						}
					}
				}

				// Removes alert from queue.
				m_session.pop_alert();
			}
		}
	} catch (boost::thread_interrupted& e) {
		// Thread will stop.
	}
}

void VideoTorrentManager::notify_playback() {
	// If deadlines algorithm is being used, updates piece deadline.
	if (m_deadlines_mode) {
		int last_requested_piece = m_video_buffer->get_next_piece_index();
		int pieces_on_buffer = last_requested_piece - m_last_played_piece;

		int buffer_time = m_decoded_piece_length * pieces_on_buffer;

		for (int i = last_requested_piece; i < m_num_pieces; i++) {
			int deadline = (i - last_requested_piece) * m_decoded_piece_length
					+ buffer_time;
			m_torrent_handle.set_piece_deadline(i, deadline);
		}

		// Disables strict sequential mode.
		m_torrent_handle.set_sequential_download(false);
	}
}

void VideoTorrentManager::notify_stall() {
	if (m_deadlines_mode) {
		m_last_played_piece = m_video_buffer->get_next_piece_index();

		// Returns to sequential mode until buffer is full.
		m_torrent_handle.set_sequential_download(true);
	}
}

Status VideoTorrentManager::get_status() {
	torrent_status t_status = m_torrent_handle.status();

	Status status;
	status.download_rate = t_status.download_payload_rate;
	status.upload_rate = t_status.upload_payload_rate;
	status.download_progress = t_status.progress;
	status.num_pieces = t_status.num_pieces;
	status.num_peers = t_status.list_peers;
	status.num_seeds = t_status.list_seeds;
	status.num_connected_peers = t_status.num_peers;
	status.num_connected_seeds = t_status.num_seeds;
	status.num_uploads = t_status.num_uploads;
	status.distributed_copies = t_status.distributed_full_copies;
	status.seconds_to_next_announce = t_status.next_announce.seconds();

	int num_pieces = t_status.pieces.size();
	status.pieces = boost::dynamic_bitset<>(num_pieces);

	for (int i = 0; i < num_pieces; i++) {
		status.pieces[num_pieces - 1 - i] = t_status.pieces[i];
	}

	return status;
}

torrent_info* VideoTorrentManager::read_torrent_file(
		const std::string& file_name) {

	int size;
	char* memory_block;
	torrent_info* ti;

	std::ifstream torrent_file(file_name.c_str(),
			std::ios::in | std::ios::binary | std::ios::ate);

	if (torrent_file.is_open()) {
		size = torrent_file.tellg();
		memory_block = new char[size];

		torrent_file.seekg(0, std::ios::beg);
		torrent_file.read(memory_block, size);
		torrent_file.close();

		ti = new torrent_info(memory_block, size);

		delete[] memory_block;

	} else {
		throw Exception("Could not open torrent file.");
	}

	return ti;
}

} /* namespace btstream */
