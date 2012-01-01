/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BTStream.
 * BTStream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BTStream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * VideoBuffer.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "VideoBuffer.h"

#include <boost/lexical_cast.hpp>


namespace btstream {

VideoBuffer::VideoBuffer(int num_pieces) throw(Exception) :
				m_next_piece_index(0) {
	if (num_pieces > 0) {
		m_pieces = std::vector<boost::shared_ptr<btstream::Piece> >(num_pieces);

	} else {
		throw Exception("Invalid buffer size: " +
				boost::lexical_cast<std::string>(num_pieces));
	}
}

void VideoBuffer::add_piece(int index, boost::shared_array<char> data, int size)
				throw(Exception) {
	if (index >= 0 && index < ((int) m_pieces.size()) && data && size > 0) {
		boost::shared_ptr<Piece> piece(new Piece(index, data, size));
		bool is_next_piece;

		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			m_pieces[index] = piece;

			is_next_piece = (index == m_next_piece_index);
		}

		if (is_next_piece) {
			m_condition.notify_all();
		}

	} else {
		throw Exception("Invalid piece: " +
				boost::lexical_cast<std::string>(index) + ", " +
				boost::lexical_cast<std::string>(size));
	}
}

boost::shared_ptr<Piece> VideoBuffer::get_next_piece() {
	boost::shared_ptr<Piece> piece;

	boost::unique_lock<boost::mutex> lock(m_mutex);
	if (m_next_piece_index < (int) m_pieces.size()) {
		piece = m_pieces[m_next_piece_index];
		while (!piece) {
			m_condition.wait(lock);
			piece = m_pieces[m_next_piece_index];
		}

		m_next_piece_index++;
	}

	return piece;
}

} /* namespace btstream */
