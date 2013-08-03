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
 * BTStreamTest.cpp
 *
 *  Created on: 04/12/2011
 *      Author: gabriel
 */

#include "btstream.h"

#include <gtest/gtest.h>

#include "exception.h"

namespace btstream {

TEST(BTStreamTest, CreateWithInvalidTorrent) {
	std::string path = "";
	ASSERT_THROW(BTStream btstream(path), Exception);
}

TEST(BTStreamTest, CreateWithValidTorrent) {
	std::string TEST_TORRENT = "testfile.torrent";
	ASSERT_NO_THROW(BTStream btstream(TEST_TORRENT));
}

TEST(BTStreamTest, AddInvalidTorrent) {
	BTStream btstream;
	std::string path = "";

	ASSERT_THROW(btstream.add_torrent(path), Exception);
}

TEST(BTStreamTest, AddValidTorrent) {
	BTStream btstream;
	std::string TEST_TORRENT = "testfile.torrent";

	ASSERT_NO_THROW(btstream.add_torrent(TEST_TORRENT));
}

TEST(BTStreamTest, GetPieceWithTorrent) {
	std::string TEST_TORRENT = "testfile.torrent";
	BTStream btstream(TEST_TORRENT);

	boost::shared_ptr<Piece> piece;

	for (int i = 0; i < 256; i++) {
		piece = btstream.get_next_piece();
		ASSERT_TRUE(piece);
		EXPECT_EQ(i, piece->index);
		EXPECT_EQ(1024, piece->size);
	}

	// After last piece returned, expect NULL
	piece = btstream.get_next_piece();
	ASSERT_FALSE(piece);
}

} /* namespace btstream */
