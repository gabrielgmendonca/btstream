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
 * VideoBufferTest.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "videobuffer.h"

#include <gtest/gtest.h>
#include <boost/thread.hpp>

#include "exception.h"

namespace btstream {

/**
 * Fills given buffer with pieces.
 * Called at a producer thread for concurrency testing.
 */
void fill_buffer(VideoBuffer *video_buffer, int num_pieces) {
	for (int i = 0; i < num_pieces; i++) {
		int index = i;
		boost::shared_array<char> data(new char[1]);
		data[0] = i;

		video_buffer->add_piece(index, data, 1);
	}
}

/**
 * Reads all pieces on VideoBuffer.
 * Called at a consumer thread for concurrency testing.
 */
void read_pieces(VideoBuffer* video_buffer, int num_pieces) {
	for (int i = 0; i < num_pieces; i++) {
		boost::shared_ptr<Piece> piece = video_buffer->get_next_piece();

		char expected_data = i;
		EXPECT_EQ(expected_data, piece->data[0]);
	}
}

void read_one_piece(VideoBuffer* video_buffer, boost::shared_ptr<Piece> piece) {
	piece = video_buffer->get_next_piece();
}

TEST(VideoBufferTest, CreateWithNegativeSize) {
	ASSERT_THROW(VideoBuffer video_buffer(-1), Exception);
}

TEST(VideoBufferTest, AddPieceInvalidIndex) {
	int buffer_length = 1;
	VideoBuffer video_buffer(buffer_length);

	int size = 1;
	boost::shared_array<char> data(new char[size]);

	ASSERT_NO_THROW(video_buffer.add_piece(0, data, size));
	EXPECT_THROW(video_buffer.add_piece(-1, data, size), Exception);
	EXPECT_THROW(video_buffer.add_piece(buffer_length, data, size), Exception);
}

TEST(VideoBufferTest, AddPieceNoData) {
	VideoBuffer video_buffer(1);

	int index = 0;
	int size = 1;
	boost::shared_array<char> data;

	EXPECT_THROW(video_buffer.add_piece(index, data, size), Exception);
}

TEST(VideoBufferTest, AddPieceInvalidSize) {
	VideoBuffer video_buffer(1);

	int index = 0;
	boost::shared_array<char> data(new char[1]);

	EXPECT_THROW(video_buffer.add_piece(index, data, 0), Exception);
	EXPECT_THROW(video_buffer.add_piece(index, data, -1), Exception);
}

TEST(VideoBufferTest, AddPieceOne) {
	VideoBuffer video_buffer(1);

	int index = 0;
	int size = 1;
	boost::shared_array<char> data(new char[size]);
	data[0] = 7;

	video_buffer.add_piece(index, data, size);

	boost::shared_ptr<Piece> piece = video_buffer.get_next_piece();

	ASSERT_TRUE(piece);
	EXPECT_EQ(index, piece->index);
	ASSERT_TRUE((bool) piece->data);
	EXPECT_EQ(data[0], piece->data[0]);
	EXPECT_EQ(size, piece->size);
}

TEST(VideoBufferTest, AddPieceTwo) {
	VideoBuffer video_buffer(2);

	int index1 = 0;
	int size1 = 1;
	boost::shared_array<char> data1(new char[size1]);
	data1[0] = 7;

	video_buffer.add_piece(index1, data1, size1);

	int index2 = 1;
	int size2 = 2;
	boost::shared_array<char> data2(new char[size2]);
	data2[0] = 3;
	data2[1] = 5;

	video_buffer.add_piece(index2, data2, size2);

	boost::shared_ptr<Piece> piece1 = video_buffer.get_next_piece();
	boost::shared_ptr<Piece> piece2 = video_buffer.get_next_piece();

	ASSERT_TRUE(piece1);
	EXPECT_EQ(index1, piece1->index);
	ASSERT_TRUE((bool) piece1->data);
	EXPECT_EQ(data1[0], piece1->data[0]);
	EXPECT_EQ(size1, piece1->size);

	ASSERT_TRUE(piece2);
	EXPECT_EQ(index2, piece2->index);
	ASSERT_TRUE((bool) piece2->data);
	EXPECT_EQ(data2[0], piece2->data[0]);
	EXPECT_EQ(data2[1], piece2->data[1]);
	EXPECT_EQ(size2, piece2->size);
}

TEST(VideoBufferTest, AddPieceConcurrent) {
	int num_pieces = 50000;
	VideoBuffer video_buffer(num_pieces + 1);

	// Producer thread. Will add pieces to the buffer.
	boost::thread producer_thread(fill_buffer, &video_buffer, num_pieces);

	// Consumer thread. Will read all pieces and check their values.
	boost::thread consumer_thread(read_pieces, &video_buffer, num_pieces);

	// Waiting for producer and consumer thread.
	boost::posix_time::time_duration td = boost::posix_time::seconds(1);
	EXPECT_TRUE(producer_thread.timed_join(td));
	EXPECT_TRUE(consumer_thread.timed_join(td));

	// Stop threads in case they are still running.
	producer_thread.interrupt();
	consumer_thread.interrupt();
	producer_thread.join();
	consumer_thread.join();


	// Adding last piece.
	int index = num_pieces;
	int size = 1;
	boost::shared_array<char> data(new char[size]);
	data[0] = 127;

	video_buffer.add_piece(index, data, size);

	// Reading last piece.
	boost::shared_ptr<Piece> last_piece = video_buffer.get_next_piece();

	ASSERT_TRUE(last_piece);
	EXPECT_EQ(index, last_piece->index);
	ASSERT_TRUE((bool) last_piece->data);
	EXPECT_EQ(data[0], last_piece->data[0]);
	EXPECT_EQ(size, last_piece->size);
}

TEST(VideoBufferTest, GetNextPieceOverflow) {
	VideoBuffer video_buffer(1);

	boost::shared_array<char> data(new char[1]);
	video_buffer.add_piece(0, data, 1);

	boost::shared_ptr<Piece> valid_piece = video_buffer.get_next_piece();
	boost::shared_ptr<Piece> invalid_piece = video_buffer.get_next_piece();

	ASSERT_TRUE(valid_piece);
	ASSERT_FALSE(invalid_piece);
}

TEST(VideoBufferTest, UnlockBeforeGetNextPiece) {
	VideoBuffer video_buffer(1);

	ASSERT_FALSE(video_buffer.unlocked());

	video_buffer.unlock();

	ASSERT_TRUE(video_buffer.unlocked());

	boost::shared_ptr<Piece> null_piece = video_buffer.get_next_piece();

	ASSERT_TRUE(video_buffer.unlocked());
	ASSERT_FALSE(null_piece);
}

TEST(VideoBufferTest, UnlockRunningGetNextPiece) {
	VideoBuffer video_buffer(1);

	boost::shared_ptr<Piece> null_piece;

	boost::thread consumer_thread(read_one_piece, &video_buffer, null_piece);

	ASSERT_FALSE(video_buffer.unlocked());

	video_buffer.unlock();

	ASSERT_TRUE(video_buffer.unlocked());

	// Threads shoudn't take too long to stop.
	boost::posix_time::time_duration td = boost::posix_time::seconds(1);
	EXPECT_TRUE(consumer_thread.timed_join(td));

	// Stop consumer thread in case it is still running.
	consumer_thread.interrupt();
	consumer_thread.join();

	ASSERT_FALSE(null_piece);
}

} /* namespace btstream */
