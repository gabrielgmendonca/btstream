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
 * VideoTorrentManager.h
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#ifndef VIDEOTORRENTMANAGER_H_
#define VIDEOTORRENTMANAGER_H_

#include <libtorrent/session.hpp>

#include "VideoBuffer.h"
#include "Exception.h"

using namespace libtorrent;

namespace bivod {

/**
 * Manages video torrents through libtorrent.
 * Sends downloaded pieces to a VideoBuffer in order to be played.
 */
class VideoTorrentManager {
public:
	VideoTorrentManager();

	/**
	 * Initializes session for specified torrent download and returns the
	 * number of pieces.
	 */
	int add_torrent(std::string file_name, std::string save_path = ".")
			throw (Exception);

	/**
	 * Starts download of the video file.
	 * The downloaded pieces will be added to the given VideoBuffer.
	 * A feeding thread will be started.
	 */
	void start_download(VideoBuffer* video_buffer) throw (Exception);

	/**
	 * Adds downloaded pieces to VideoBuffer.
	 * Pieces will be get through a libtorrent alert.
	 */
	void feed_video_buffer();

private:
	session m_session;
	torrent_handle m_torrent_handle;
	VideoBuffer* m_video_buffer;
	int m_pieces_to_play;
};

} /* namespace bivod */

#endif /* VIDEOTORRENTMANAGER_H_ */
