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
 * VideoTorrentManager.cpp
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#include "videotorrentmanager.h"

#include <fstream>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/peer_info.hpp>
#include <libtorrent/bencode.hpp>

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
			libtorrent::alert::storage_notification
					| libtorrent::alert::progress_notification);
}

VideoTorrentManager::~VideoTorrentManager() {
	if (m_feeding_thread) {
		// Stops feeding thread.
		m_feeding_thread->interrupt();
		m_feeding_thread->join();
	}

	// Stops session.
	m_session.pause();

	save_resume_data();
}

boost::shared_ptr<VideoBuffer> VideoTorrentManager::add_torrent(
		const std::string& file_name, const std::string& save_path,
		Algorithm algorithm, int stream_length) throw (Exception) {

	add_torrent(file_name, 0, save_path);

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

	return m_video_buffer;
}

boost::shared_ptr<VideoBuffer> VideoTorrentManager::add_torrent(
		const std::string& file_name, PiecePicker* piece_picker,
		const std::string& save_path) throw (Exception) {

	int num_pieces = 0;
	try {
		libtorrent::add_torrent_params params;
		params.ti = read_torrent_file(file_name);
		params.save_path = save_path;
//		params.auto_managed = false;
		params.userdata = dynamic_cast<void*>(piece_picker);

		std::string video_file_name = params.ti->name() + ".resume";
		std::string resume_data_file = save_path + "/" + video_file_name;

		std::vector<char> buffer;
		libtorrent::error_code ec;

		if (libtorrent::load_file(resume_data_file.c_str(), buffer, ec) == 0) {
			params.resume_data = &buffer;
		}

		m_torrent_handle = m_session.add_torrent(params);

		m_save_path = save_path;
		m_num_pieces = params.ti.get()->num_pieces();
		m_next_piece = 0;

		m_video_buffer =
				boost::shared_ptr<VideoBuffer>(new VideoBuffer(m_num_pieces));

		// Starts VideoBuffer feeding thread that calls the feed_video_buffer method.
		m_feeding_thread = boost::shared_ptr<boost::thread>(
				new boost::thread(&VideoTorrentManager::feed_video_buffer, this));

	} catch (std::exception& e) {
		throw Exception(e.what());
	}

	return m_video_buffer;
}

void VideoTorrentManager::feed_video_buffer() {
	try {
		while (m_next_piece < m_num_pieces) {

			// Tries to get an alert from alert queue.
			const libtorrent::alert* new_alert = m_session.wait_for_alert(
					libtorrent::seconds(30));

			if (new_alert) {
				// Tries to cast alert pointer to different alert types.
				const libtorrent::piece_finished_alert* finished_alert =
						libtorrent::alert_cast<libtorrent::piece_finished_alert>(
								new_alert);
				const libtorrent::read_piece_alert* read_alert =
						libtorrent::alert_cast<libtorrent::read_piece_alert>(
								new_alert);

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
	libtorrent::torrent_status t_status = m_torrent_handle.status();

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

libtorrent::torrent_info* VideoTorrentManager::read_torrent_file(
		const std::string& file_name) {

	int size;
	char* memory_block;
	libtorrent::torrent_info* ti = 0;

	std::ifstream torrent_file(file_name.c_str(),
			std::ios::in | std::ios::binary | std::ios::ate);

	if (torrent_file.is_open()) {
		size = torrent_file.tellg();
		memory_block = new char[size];

		torrent_file.seekg(0, std::ios::beg);
		torrent_file.read(memory_block, size);
		torrent_file.close();

		ti = new libtorrent::torrent_info(memory_block, size);

		delete[] memory_block;

	} else {
		throw Exception("Could not open torrent file " + file_name);
	}

	return ti;
}

void VideoTorrentManager::save_resume_data() {
	if (m_torrent_handle.is_valid()) {
		m_torrent_handle.pause();
		m_torrent_handle.save_resume_data();

		// Waits for alerts.
		int outstanding_resume_data = 1;

		while (outstanding_resume_data > 0) {
			libtorrent::alert const* new_alert = m_session.wait_for_alert(
					libtorrent::seconds(20));

			if (new_alert) {
				libtorrent::save_resume_data_alert const* resume_alert =
						libtorrent::alert_cast<libtorrent::save_resume_data_alert>(
								new_alert);

				// Saves resume data file.
				if (resume_alert) {
					std::string save_path = m_save_path;
					std::string torrent_name =
							m_torrent_handle.get_torrent_info().name();
					std::string resume_path = save_path + "/" + torrent_name
							+ ".resume";

					std::ofstream out(resume_path.c_str(), std::ios_base::binary);
					out.unsetf(std::ios_base::skipws);
					bencode(std::ostream_iterator<char>(out),
							*resume_alert->resume_data);
					outstanding_resume_data--;
				}

				// Resume data failed.
				if (libtorrent::alert_cast<libtorrent::save_resume_data_failed_alert>(
						new_alert)) {
					outstanding_resume_data--;
				}

				m_session.pop_alert();
			}
		}
	}
}

} /* namespace btstream */
