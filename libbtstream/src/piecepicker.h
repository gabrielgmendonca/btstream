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
 * PiecePicker.h
 *
 *  Created on: 11/03/2012
 *      Author: gabriel
 */

#ifndef PIECEPICKER_H_
#define PIECEPICKER_H_

#include <libtorrent/torrent.hpp>

namespace btstream {

class PiecePicker {
public:

	PiecePicker();

	/**
	 * Adds a new piece request to the download queue.
	 * The piece selection is done by the pick_piece method.
	 */
	virtual void add_piece_request(libtorrent::torrent* t);

	/**
	 * Adds first requests to download queue.
	 * The piece selection is done by the pick_piece method.
	 */
	virtual void init(libtorrent::torrent* t);

	virtual ~PiecePicker() {};

protected:

	/**
	 * Returns the index of the next piece that should be requested.
	 */
	virtual int pick_piece(libtorrent::torrent* t) = 0;

private:
	int m_deadline;
};

} /* namespace btstream */
#endif /* PIECEPICKER_H_ */
