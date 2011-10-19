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
 * VideoBufferTest.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "VideoBuffer.h"

#include <gtest/gtest.h>
#include <boost/thread.hpp>

#include "Exception.h"

namespace bivod {

/**
 * Fills given buffer with pieces in pseudo-random order.
 */
void fill_buffer(VideoBuffer *video_buffer, int buffer_size) {
	int prime = 131071;
	int index = random() % buffer_size;
	for (int i = 0; i < buffer_size; i++) {
		boost::shared_array<char> data(new char[1]);
		data[0] = index;

		video_buffer->add_piece(index, data, 1);
		index = (index + prime) % buffer_size;
	}
}

/**
 * Reads all pieces on VideoBuffer.
 * Called at a consumer thread for concurrency testing.
 */
void read_pieces(VideoBuffer *video_buffer, int buffer_size) {
	for (int i = 0; i < buffer_size; i++) {
		boost::shared_ptr<Piece> piece = video_buffer->get_next_piece();

		char expected_data = piece->index;
		EXPECT_EQ(expected_data, piece->data[0]);
	}
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

	int index2 = 1;
	int size2 = 2;
	boost::shared_array<char> data2(new char[size2]);
	data2[0] = 3;
	data2[1] = 5;

	video_buffer.add_piece(index2, data2, size2);

	int index1 = 0;
	int size1 = 1;
	boost::shared_array<char> data1(new char[size1]);
	data1[0] = 7;

	video_buffer.add_piece(index1, data1, size1);

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
	int buffer_size = 50000;
	VideoBuffer video_buffer(buffer_size + 1);

	fill_buffer(&video_buffer, buffer_size);

	// Consumer thread. Will read all pieces and check their values.
	boost::thread consumer_thread(read_pieces, &video_buffer, buffer_size);

	// Adding last piece in parallel.
	int index = buffer_size;
	int size = 1;
	boost::shared_array<char> data(new char[size]);
	data[0] = 127;

	video_buffer.add_piece(index, data, size);

	// Waiting for consumer to finish reading.
	consumer_thread.join();


	// Reading last piece.
	boost::shared_ptr<Piece> last_piece = video_buffer.get_next_piece();

	ASSERT_TRUE(last_piece);
	EXPECT_EQ(index, last_piece->index);
	ASSERT_TRUE((bool) last_piece->data);
	EXPECT_EQ(data[0], last_piece->data[0]);
	EXPECT_EQ(size, last_piece->size);
}

TEST(VideoBufferTest, AddPieceDeadlock) {
	int buffer_size = 50000;
	VideoBuffer video_buffer(buffer_size);

	// Consumer thread. Will read all pieces and check their values.
	boost::thread consumer_thread(read_pieces, &video_buffer, buffer_size);

	// Producer thread. Will add pieces to VideoBuffer.
	boost::thread producer_thread(fill_buffer, &video_buffer, buffer_size);

	// Threads shoudn't take to long to stop.
	boost::posix_time::time_duration td = boost::posix_time::seconds(1);
	EXPECT_TRUE(producer_thread.timed_join(td));
	EXPECT_TRUE(consumer_thread.timed_join(td));
}

} /* namespace bivod */
