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

#include "BTStream.h"

#include <gtest/gtest.h>

#include "Exception.h"

namespace btstream {

TEST(BTStreamTest, CreateWithInvalidTorrent) {
	std::string path = "";
	ASSERT_THROW(BTStream btstream(path), Exception);
}

TEST(BTStreamTest, CreateWithValidTorrent) {
	std::string TEST_TORRENT = "test/big_buck_bunny.torrent";
	ASSERT_NO_THROW(BTStream btstream(TEST_TORRENT));
}

TEST(BTStreamTest, GetPiece) {
	std::string TEST_TORRENT = "test/big_buck_bunny.torrent";
	BTStream btstream(TEST_TORRENT);

	boost::shared_ptr<Piece> piece;
	piece = btstream.get_next_piece();
	ASSERT_TRUE(piece);
	EXPECT_EQ(0, piece->index);

	piece = btstream.get_next_piece();
	ASSERT_TRUE(piece);
	EXPECT_EQ(1, piece->index);
}

} /* namespace btstream */
