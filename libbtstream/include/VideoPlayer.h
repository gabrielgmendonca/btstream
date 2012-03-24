/*
 * Copyright (C) 2011-2012 Gabriel Mendonça
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
 * VideoPlayer.h
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_

#include <boost/shared_array.hpp>

namespace btstream {

class VideoPlayer {
public:
	VideoPlayer();

	/**
	 * Adds a downloaded piece to buffer.
	 */
	void add_piece(int index, boost::shared_array<char> data, int size);
};

} /* namespace btstream */

#endif /* VIDEOPLAYER_H_ */
