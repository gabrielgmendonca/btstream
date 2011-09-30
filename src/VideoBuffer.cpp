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
 * VideoBuffer.cpp
 *
 *  Created on: 29/09/2011
 *      Author: gabriel
 */

#include "VideoBuffer.h"

namespace bivod {

VideoBuffer::VideoBuffer(int num_pieces) :
		m_pieces(num_pieces) {}

void VideoBuffer::add_piece(int index, boost::shared_array<char> data, int size) {
	boost::shared_ptr<Piece> piece(new Piece(index, data, size));

	{
		boost::lock_guard lock(m_mutex);
		m_pieces[index] = piece;
	}

	m_condition.notify_all();
}

} /* namespace bivod */
