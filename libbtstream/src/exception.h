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
 * Exception.h
 *
 *  Created on: 16/10/2011
 *      Author: gabriel
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>

namespace btstream {

/**
 * Base exception class.
 */
class Exception: public std::exception {
public:
	Exception(std::string message) throw();
	virtual ~Exception() throw();
	virtual const char* what() const throw();

private:
	std::string m_message;
};

} /* namespace btstream */
#endif /* EXCEPTION_H_ */
