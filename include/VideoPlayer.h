/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BIVoD.
 * BIVoD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BIVoD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BIVoD.  If not, see <http://www.gnu.org/licenses/>.
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

namespace bivod {

class VideoPlayer {
public:
	VideoPlayer();

	/**
	 * Adds a downloaded piece to buffer.
	 */
	void add_piece(int index, boost::shared_array<char> data, int size);
};

} /* namespace bivod */

#endif /* VIDEOPLAYER_H_ */
