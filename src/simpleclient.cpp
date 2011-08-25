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
 * simpleclient.cpp
 *
 *  Created on: 24/08/2011
 *      Author: gabriel
 */

#include <iostream>
#include <libtorrent/session.hpp>

using namespace libtorrent;

int main(int argc, char **argv) {

	if (argc != 2) {
		std::cerr << "usage: ./BIVoD torrent-file\n";
		return 1;
	}

	session s;

	add_torrent_params p;
	p.save_path = "./";
	error_code ec;
	p.ti = new torrent_info(argv[1], ec);

	if (ec) {
		std::cout << ec.message() << std::endl;
		return 1;
	}
	s.add_torrent(p, ec);
	if (ec) {
		std::cerr << ec.message() << std::endl;
		return 1;
	}

	// wait for the user to end
	char a;
	std::cin.unsetf(std::ios_base::skipws);
	std::cin >> a;

	return 0;
}
