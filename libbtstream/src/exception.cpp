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
 * Exception.cpp
 *
 *  Created on: 16/10/2011
 *      Author: gabriel
 */

#include "exception.h"

namespace btstream {

Exception::Exception(std::string message) throw():
		m_message(message) {}

Exception::~Exception() throw() {}

const char* Exception::what() const throw() {
	return m_message.c_str();
}

} /* namespace btstream */
