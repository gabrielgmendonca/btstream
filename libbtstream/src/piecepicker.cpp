/*
 * PiecePicker.cpp
 *
 *  Created on: 11/03/2012
 *      Author: gabriel
 */

#include "piecepicker.h"

namespace btstream {

PiecePicker::PiecePicker(): m_deadline(0) {}

void PiecePicker::add_piece_request(libtorrent::torrent* t) {
	if (t) {
		int piece_index = pick_piece(t);

		t->set_piece_deadline(piece_index, m_deadline++, 0);
	}
}

void PiecePicker::init(libtorrent::torrent* t) {
	if (t) {
		// TODO: Change to the max number of parallel requests.
		int n = 10;

		for (int i = 0; i < n; i++) {
			add_piece_request(t);
		}
	}
}

} /* namespace btstream */
