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
 * VideoTorrentManagerTest.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "videotorrentmanager.h"

#include <gtest/gtest.h>

#include "exception.h"

namespace btstream {

TEST(VideoTorrentManagerTest, AddTorrentInvalid) {
	VideoTorrentManager video_torrent_manager;

	ASSERT_THROW(video_torrent_manager.add_torrent("", 0, "."), Exception);
}

TEST(VideoTorrentManagerTest, AddTorrentValid) {
	VideoTorrentManager video_torrent_manager;

	int piece_number = 256;
	std::string TEST_TORRENT = "testfile1.torrent";
	ASSERT_EQ(piece_number, video_torrent_manager.add_torrent(TEST_TORRENT, 0, "."));
}

} /* namespace btstream */
