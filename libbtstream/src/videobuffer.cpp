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
 * VideoBuffer.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "videobuffer.h"

#include <boost/lexical_cast.hpp>

namespace btstream {

VideoBuffer::VideoBuffer(int num_pieces) throw (Exception) :
		m_buffer_size(10), m_num_pieces(num_pieces), m_next_piece_index(0),
		m_unlocked(false) {

	if (num_pieces <= 0) {
		throw Exception("Invalid number of pieces.");
	}
}

VideoBuffer::~VideoBuffer() {
	unlock();
}

void VideoBuffer::add_piece(int index, boost::shared_array<char> data, int size) {

	if (index >= 0 && index < m_num_pieces && data && size > 0) {
		boost::unique_lock<boost::mutex> lock(m_mutex);

		// Waits while buffer is full.
		while (m_pieces.size() == m_buffer_size) {
			m_buffer_not_full.wait(lock);
		}

		// Adds piece to buffer.
		boost::shared_ptr<Piece> piece(new Piece(index, data, size));
		m_pieces.push(piece);

		// Notifies that next piece is available.
		m_next_piece_available.notify_all();

	} else {
		throw Exception(
				"Invalid piece: " + boost::lexical_cast<std::string>(index)
						+ ", " + boost::lexical_cast<std::string>(size));
	}
}

boost::shared_ptr<Piece> VideoBuffer::get_next_piece() {
	boost::shared_ptr<Piece> piece;

	boost::unique_lock<boost::mutex> lock(m_mutex);
	if (m_next_piece_index < m_num_pieces) {
		while (m_pieces.empty() && !m_unlocked) {
			m_next_piece_available.wait(lock);
		}

		if (m_unlocked) {
			boost::shared_ptr<Piece> null_pointer;
			return null_pointer;
		}

		piece = m_pieces.front();
		m_pieces.pop();

		m_next_piece_index++;

		// Notifies that there is free space on buffer.
		m_buffer_not_full.notify_all();
	}

	return piece;
}

int VideoBuffer::get_next_piece_index() {
	boost::lock_guard<boost::mutex> lock(m_mutex);
	return m_next_piece_index;
}

void VideoBuffer::unlock() {
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_unlocked = true;
	} // Releasing lock.

	m_next_piece_available.notify_all();
}

bool VideoBuffer::unlocked() {
	boost::lock_guard<boost::mutex> lock(m_mutex);
	return m_unlocked;
}

} /* namespace btstream */
