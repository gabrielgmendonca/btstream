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
 * BTStream.h
 *
 *  Created on: 04/12/2011
 *      Author: gabriel
 */

#ifndef BTSTREAM_H_
#define BTSTREAM_H_

#include <string>
#include <boost/shared_ptr.hpp>

#include "VideoTorrentManager.h"
#include "VideoBuffer.h"

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
	 * @param save_path
	 * 			Path where the downloaded file will be stored.
	 * @param sequential_download
	 * 			Whether to use sequential download or rarest-first algorithm.
	 * @param seed_ip
	 * 			IP address of a previously known seed.
	 * @param seed_port
	 * 			Port to connect on the previously known seed.
	 */
	BTStream(const std::string& torrent_path, const std::string save_path = ".",
			bool sequential_download = true, const std::string seed_ip = "",
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
	boost::shared_ptr<VideoTorrentManager> m_video_torrent_manager;
	boost::shared_ptr<VideoBuffer> m_video_buffer;
};

} /* namespace btstream */
#endif /* BTSTREAM_H_ */
