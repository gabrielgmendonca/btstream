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
 * BTStream.h
 *
 *  Created on: 04/12/2011
 *      Author: gabriel
 */

#ifndef BTSTREAM_H_
#define BTSTREAM_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "videotorrentmanager.h"
#include "videobuffer.h"

namespace btstream {

/**
 * This is BTStream library's base class.
 * In order to receive a stream from a BitTorrent swarm, one just need
 * to provide a path to a valid torrent file at the constructor and,
 * after that, call the get_next_piece method regularly to read
 * downloaded pieces.
 */
class BTStream {
public:

	/**
	 * Constructor.
	 * @param torrent_path
	 * 			Path to a valid torrent file.
	 * @param algorithm
	 * 			Built-in piece picking algorithm that will be used.
	 * @param stream_length
	 * 			Length of the decoded stream in milliseconds.
	 * 			It must be provided when using the DEADLINE algorithm.
	 * @param save_path
	 * 			Path where the downloaded file will be stored.
	 * @param seed_ip
	 * 			IP address of a previously known seed.
	 * @param seed_port
	 * 			Port to connect on the previously known seed.
	 */
	BTStream(const std::string& torrent_path,
			Algorithm algorithm = RAREST_FIRST, int stream_length = 0,
			const std::string& save_path = ".", const std::string& seed_ip = "",
			unsigned short seed_port = 0);

	/**
	 * Constructor.
	 * @param torrent_path
	 * 			Path to a valid torrent file.
	 * @param piece picker
	 * 			Pointer to a custom PiecePicker.
	 * @param save_path
	 * 			Path where the downloaded file will be stored.
	 * @param seed_ip
	 * 			IP address of a previously known seed.
	 * @param seed_port
	 * 			Port to connect on the previously known seed.
	 */
	BTStream(const std::string& torrent_path, PiecePicker* piece_picker,
			const std::string& save_path = ".", const std::string& seed_ip = "",
			unsigned short seed_port = 0);

	/**
	 * Returns a pointer to the next piece that should be played.
	 *
	 * If all pieces have already been returned or the unlock() method
	 * was called, returns a default constructed (NULL) shared_ptr.
	 *
	 * This method will block (sleep) until the piece is available or
	 * the unlock() method is called.
	 */
	boost::shared_ptr<Piece> get_next_piece();

	/**
	 * Returns a Status object with data like download rate, upload
	 * rate and progress.
	 */
	Status get_status();

    /**
     * Notifies BTStream that video player's playback buffer is full
     * and playback will start/resume. This way piece selection can be
     * optimized in order to avoid stalls.
     */
    void notify_playback();

    /**
     * Notifies BTStream that video player's playback buffer is empty
     * and playback will stall. This way piece selection can be 
     * optimized in order to avoid other stalls during playback.
     */
    void notify_stall();

	/**
	 * Unlocks any blocked calls to get_next_piece().
	 */
	void unlock();

	/**
	 * Returns true if unlock() was called and false otherwise.
	 * If false, a call to get_next_piece() may be blocked waiting for
	 * the next piece to be downloaded.
	 */
	bool unlocked();

private:

	void init(int num_pieces);

	boost::shared_ptr<VideoTorrentManager> m_video_torrent_manager;
	boost::shared_ptr<VideoBuffer> m_video_buffer;
};

} /* namespace btstream */
#endif /* BTSTREAM_H_ */
