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
	 * @param torrent_path Path to a valid torrent file.
	 */
	BTStream(std::string& torrent_path);

	/**
	 * Returns a pointer to the next piece that should be played.
	 * If all pieces have already been returned, returns a default
	 * constructed (NULL) shared_ptr.
	 * This method will block until the piece is available.
	 */
	boost::shared_ptr<Piece> get_next_piece();

private:
	boost::shared_ptr<VideoTorrentManager> m_video_torrent_manager;
	boost::shared_ptr<VideoBuffer> m_video_buffer;
};

} /* namespace btstream */
#endif /* BTSTREAM_H_ */
