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
 * SequentialPiecePicker.cpp
 *
 *  Created on: 11/03/2012
 *      Author: gabriel
 */

#include "sequentialpiecepicker.h"

namespace btstream {

SequentialPiecePicker::SequentialPiecePicker() :
		m_counter(0) {
}

int SequentialPiecePicker::pick_piece(torrent* t) {
	if (m_counter < t->torrent_file().num_pieces()) {
		return m_counter++;
	} else {
		return m_counter - 1;
	}
}

} /* namespace btstream */
