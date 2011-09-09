/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BIVoD.
 * BIVoD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BIVoD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BIVoD.  If not, see <http://www.gnu.org/licenses/>.
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

#include "VideoPlayer.h"

using namespace libtorrent;

namespace bivod {

/**
 * Manages video torrents through libtorrent.
 * Sends downloaded pieces to a VideoPlayer in order to be played.
 */
class VideoTorrentManager {
public:
	VideoTorrentManager(VideoPlayer* video_player);

	/**
	 * Starts the download of the specified torrent.
	 * The feeding thread will be started.
	 */
	void add_torrent(std::string file_name, std::string save_path);

	/**
	 * Sends downloaded pieces to VideoPlayer.
	 * Pieces will be get through a libtorrent alert.
	 */
	void feed_video_player();

private:
	session m_session;
	VideoPlayer* m_video_player;
};

} /* namespace bivod */

#endif /* VIDEOTORRENTMANAGER_H_ */
