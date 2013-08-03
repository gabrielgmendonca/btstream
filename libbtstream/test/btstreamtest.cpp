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

std::string TEST_TORRENT = "testfile.torrent";
int TEST_TORRENT_PIECES = 256;
int TEST_TORRENT_PIECE_SIZE = 1024;

TEST(BTStreamTest, CreateWithInvalidTorrent) {
	ASSERT_THROW(BTStream btstream(""), Exception);
}

TEST(BTStreamTest, CreateWithValidTorrent) {
	ASSERT_NO_THROW(BTStream btstream(TEST_TORRENT));
}

TEST(BTStreamTest, AddInvalidTorrent) {
	BTStream btstream;
	ASSERT_THROW(btstream.add_torrent(""), Exception);
}

TEST(BTStreamTest, AddValidTorrent) {
	BTStream btstream;
	ASSERT_NO_THROW(btstream.add_torrent(TEST_TORRENT));
}

TEST(BTStreamTest, GetPieceWithTorrent) {
	BTStream btstream(TEST_TORRENT);

	boost::shared_ptr<Piece> piece;

	for (int i = 0; i < TEST_TORRENT_PIECES; i++) {
		piece = btstream.get_next_piece();
		ASSERT_TRUE(piece);
		EXPECT_EQ(i, piece->index);
		EXPECT_EQ(TEST_TORRENT_PIECE_SIZE, piece->size);
	}

	// After last piece returned, expect NULL
	piece = btstream.get_next_piece();
	ASSERT_FALSE(piece);
}

TEST(BTStreamTest, GetStatus) {
	BTStream btstream(TEST_TORRENT);

	// Wait until pieces have been read.
	btstream.get_next_piece();

	Status status = btstream.get_status();

	EXPECT_EQ(0, status.download_rate);
	EXPECT_EQ(1.0f, status.download_progress);
	EXPECT_EQ(TEST_TORRENT_PIECES, status.num_pieces);
	EXPECT_EQ(1, status.num_peers);
	EXPECT_EQ(0, status.num_seeds);
	EXPECT_EQ(0, status.num_connected_peers);
	EXPECT_EQ(0, status.num_connected_seeds);
	EXPECT_EQ(0, status.num_uploads);
	EXPECT_EQ(-1, status.distributed_copies);
	EXPECT_EQ(0, status.seconds_to_next_announce);

	ASSERT_EQ(TEST_TORRENT_PIECES, status.pieces.size());

	boost::dynamic_bitset<> bitset(TEST_TORRENT_PIECES);
	bitset.flip();
	EXPECT_EQ(bitset, status.pieces);
}

} /* namespace btstream */
