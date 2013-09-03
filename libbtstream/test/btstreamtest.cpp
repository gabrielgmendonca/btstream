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

#include <gtest/gtest.h>

#include "btstream.h"
#include "exception.h"

#include "constants.h"

namespace btstream {

/**
 * Simulates video playback.
 * Called at a separate thread.
 */
void read_pieces(BTStream* btstream, int num_pieces, int piece_length,
	bool check) {

	for (int i = 0; i < num_pieces; i++) {
		boost::shared_ptr<Piece> piece;

		piece = btstream->get_next_piece();

		if (check) {
			ASSERT_TRUE(piece);
			EXPECT_EQ(i, piece->index);
			EXPECT_EQ(piece_length, piece->size);
		}
	}

	if (check) {
		// After last piece returned, expect NULL
		EXPECT_FALSE(btstream->get_next_piece());
	}
}

void run_playback_thread(BTStream* btstream, int num_pieces, int
	piece_length, bool check) {

	// Start playback thread.
	boost::thread playback_thread(read_pieces, btstream, num_pieces,
			piece_length, check);

	// Wait for playback thread to end.
	boost::posix_time::time_duration td = boost::posix_time::seconds(20);
	EXPECT_TRUE(playback_thread.timed_join(td));

	// Stop playback thread in case it is still running.
	playback_thread.interrupt();
	playback_thread.join();
}

TEST(BTStreamTest, CreateWithInvalidTorrent) {
	ASSERT_THROW(BTStream btstream(""), Exception);
}

TEST(BTStreamTest, CreateWithValidTorrent) {
	ASSERT_NO_THROW(BTStream btstream(TEST_TORRENT1));
}

TEST(BTStreamTest, AddInvalidTorrent) {
	BTStream btstream;
	ASSERT_THROW(btstream.add_torrent(""), Exception);
}

TEST(BTStreamTest, AddValidTorrent) {
	BTStream btstream;
	ASSERT_NO_THROW(btstream.add_torrent(TEST_TORRENT1));
}

TEST(BTStreamTest, AddTwoTorrents) {
	BTStream btstream;
	ASSERT_NO_THROW(btstream.add_torrent(TEST_TORRENT1));
	ASSERT_NO_THROW(btstream.add_torrent(TEST_TORRENT2));
}

TEST(BTStreamTest, GetPieceWithTorrent) {
	BTStream btstream(TEST_TORRENT1);

	bool check = true;
	run_playback_thread(&btstream, TEST_TORRENT1_PIECES,
		TEST_TORRENT1_PIECE_LENGTH, check);
}

TEST(BTStreamTest, GetPieceFromTwoTorrents) {
	BTStream btstream;
	bool check = true;

	// Test first torrent.
	btstream.add_torrent(TEST_TORRENT1);

	run_playback_thread(&btstream, TEST_TORRENT1_PIECES,
		TEST_TORRENT1_PIECE_LENGTH, check);


	// Test second torrent.
	btstream.add_torrent(TEST_TORRENT2);

	run_playback_thread(&btstream, TEST_TORRENT2_PIECES,
		TEST_TORRENT2_PIECE_LENGTH, check);
}

TEST(BTStreamTest, GetPieceAfterTwoTorrents) {
	BTStream btstream;
	bool check = true;

	// Add first torrent.
	btstream.add_torrent(TEST_TORRENT1);

	// Add second torrent.
	btstream.add_torrent(TEST_TORRENT2);

	// Pieces on buffer should be from torrent 2.
	run_playback_thread(&btstream, TEST_TORRENT2_PIECES,
		TEST_TORRENT2_PIECE_LENGTH, check);
}

TEST(BTStreamTest, GetStatus) {
	BTStream btstream(TEST_TORRENT1);

	bool check = false;
	run_playback_thread(&btstream, TEST_TORRENT1_PIECES,
		TEST_TORRENT1_PIECE_LENGTH, check);

	Status status = btstream.get_status();

	EXPECT_EQ(0, status.download_rate);
	EXPECT_EQ(0, status.upload_rate);
	EXPECT_EQ(1.0f, status.download_progress);
	EXPECT_EQ(TEST_TORRENT1_PIECES, status.num_pieces);
	EXPECT_EQ(0, status.num_peers);
	EXPECT_EQ(0, status.num_seeds);
	EXPECT_EQ(0, status.num_connected_peers);
	EXPECT_EQ(0, status.num_connected_seeds);
	EXPECT_EQ(0, status.num_uploads);
	EXPECT_EQ(-1, status.distributed_copies);
	EXPECT_EQ(0, status.seconds_to_next_announce);

	ASSERT_EQ(TEST_TORRENT1_PIECES, status.pieces.size());

	boost::dynamic_bitset<> bitset(TEST_TORRENT1_PIECES);
	bitset.flip();
	EXPECT_EQ(bitset, status.pieces);
}

} /* namespace btstream */
