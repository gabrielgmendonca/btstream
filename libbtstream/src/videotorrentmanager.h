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
 * VideoTorrentManager.h
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#ifndef VIDEOTORRENTMANAGER_H_
#define VIDEOTORRENTMANAGER_H_

#include <libtorrent/session.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>

#include "videobuffer.h"
#include "exception.h"
#include "piecepicker.h"

using namespace libtorrent;

namespace btstream {

struct Status {
	int download_rate;
	int upload_rate;
	float download_progress;
	boost::dynamic_bitset<> pieces;
	int num_pieces;
	int num_peers;
	int num_seeds;
	int num_connected_peers;
	int num_connected_seeds;
	int num_uploads;
	int distributed_copies;
	long seconds_to_next_announce;
};

enum Algorithm {
	RAREST_FIRST, SEQUENTIAL, DEADLINE
};

/**
 * Manages video torrents through libtorrent.
 * Sends downloaded pieces to a VideoBuffer in order to be played.
 */
class VideoTorrentManager {
public:

	/**
	 * Default constructor.
	 */
	VideoTorrentManager();

	/**
	 * Destructor.
	 */
	~VideoTorrentManager();

	/**
	 * Prepares the download of the torrent given by file_name and returns the
	 * number of pieces. The address of a previously known seed may be provided.
	 *
	 * With this method, a built-in piece selection algorithm can be chosen.
	 */
	int add_torrent(const std::string& file_name, const std::string& save_path,
			Algorithm algorithm, int stream_length) throw (Exception);

	/**
	 * Prepares the download of the torrent given by file_name and returns the
	 * number of pieces. The address of a previously known seed may be provided.
	 *
	 * With this method, a custom piece selection algorithm can be provided.
	 */
	int add_torrent(const std::string& file_name, PiecePicker* piece_picker,
			const std::string& save_path) throw (Exception);

	/**
	 * Starts download of the video file.
	 * The downloaded pieces will be added to the given VideoBuffer.
	 * A feeding thread will be started.
	 */
	void start_download(boost::shared_ptr<VideoBuffer> video_buffer)
			throw (Exception);

	/**
	 * Adds downloaded pieces to VideoBuffer.
	 * Pieces will be get through a libtorrent alert.
	 */
	void feed_video_buffer();

	/**
	 * Knowing that video player's playback buffer is full and
	 * playback will start/resume, optimizes piece selection to avoid
	 * stalls during playback.
	 */
	void notify_playback();

	/**
	 * Knowing that video player's playback buffer is empty and
	 * playback will stall, optimizes piece selection to avoid other
	 * stalls during playback.
	 */
	void notify_stall();

	/**
	 * Returns a Status object with statistics like download rate,
	 * upload rate, progress and current class.
	 */
	Status get_status();

private:

	torrent_info* read_torrent_file(const std::string& file_name);

	session m_session;
	torrent_handle m_torrent_handle;
	boost::shared_ptr<VideoBuffer> m_video_buffer;
	int m_num_pieces;
	int m_next_piece;
	int m_last_played_piece;
	bool m_deadlines_mode;
	float m_decoded_piece_length;

	boost::shared_ptr<boost::thread> m_feeding_thread;
};

} /* namespace btstream */

#endif /* VIDEOTORRENTMANAGER_H_ */
