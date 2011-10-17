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

#include "Exception.h"

namespace bivod {

TEST(VideoBufferTest, CreateWithNegativeSize) {
	ASSERT_THROW(VideoBuffer videoBuffer(-1), Exception);
}

TEST(VideoBufferTest, AddPieceNegativeIndex) {
	VideoBuffer videoBuffer(1);

	int size = 1;
	boost::shared_array<char> data(new char[size]);

	ASSERT_NO_THROW(videoBuffer.add_piece(0, data, size));
	EXPECT_THROW(videoBuffer.add_piece(-1, data, size), Exception);
}

TEST(VideoBufferTest, AddPieceNoData) {
	VideoBuffer videoBuffer(1);

	int index = 0;
	int size = 1;
	boost::shared_array<char> data;

	EXPECT_THROW(videoBuffer.add_piece(index, data, size), Exception);
}

TEST(VideoBufferTest, AddPieceInvalidSize) {
	VideoBuffer videoBuffer(1);

	int index = 0;
	boost::shared_array<char> data(new char[1]);

	EXPECT_THROW(videoBuffer.add_piece(index, data, 0), Exception);
	EXPECT_THROW(videoBuffer.add_piece(index, data, -1), Exception);
}

TEST(VideoBufferTest, AddPieceOne) {
	VideoBuffer videoBuffer(1);

	int index = 0;
	int size = 1;
	boost::shared_array<char> data(new char[size]);
	data[0] = 7;

	videoBuffer.add_piece(index, data, size);

	boost::shared_ptr<Piece> piece = videoBuffer.get_next_piece();

	ASSERT_NE((Piece*) 0, piece.get());
	EXPECT_EQ(index, piece->index);
	ASSERT_NE((char*) 0, piece->data.get());
	EXPECT_EQ(data[0], piece->data[0]);
	EXPECT_EQ(size, piece->size);
}

} /* namespace bivod */
