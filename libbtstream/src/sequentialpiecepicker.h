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
 * SequentialPiecePicker.h
 *
 *  Created on: 11/03/2012
 *      Author: gabriel
 */

#ifndef SEQUENTIALPIECEPICKER_H_
#define SEQUENTIALPIECEPICKER_H_

#include "piecepicker.h"

namespace btstream {

class SequentialPiecePicker: public PiecePicker {
public:
	SequentialPiecePicker();

	/**
	 * Returns each piece index sequentially.
	 */
	virtual int pick_piece(torrent* t);

private:
	int m_counter;
};

} /* namespace btstream */
#endif /* SEQUENTIALPIECEPICKER_H_ */
