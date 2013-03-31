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
 * VideoTorrentPlugin.h
 *
 *  Created on: 24/08/2011
 *      Author: gabriel
 */

#ifndef VIDEOTORRENTPLUGIN_H_
#define VIDEOTORRENTPLUGIN_H_

#include <boost/shared_ptr.hpp>

#include <libtorrent/extensions.hpp>
#include <libtorrent/torrent.hpp>

#include "piecepicker.h"

using namespace libtorrent;

namespace btstream {

class VideoTorrentPlugin: public torrent_plugin {
public:
	VideoTorrentPlugin(torrent* t, PiecePicker* pp = 0);

	/**
	 * If a custom PiecePicker was provided, requests the next piece.
	 * Called when a piece is received and pass the hash check.
	 */
	virtual void on_piece_pass(int index);

	virtual void on_files_checked();

private:
	torrent* m_torrent;
	boost::shared_ptr<PiecePicker> m_piece_picker;
};

/**
 * Function template for a torrent_plugin factory function.
 */
typedef boost::function<
		boost::shared_ptr<libtorrent::torrent_plugin>(libtorrent::torrent *,
				void *)> TorrentPluginFactory;

/**
 * Returns a new VideoTorrentPlugin.
 * In order to add VideoTorrentPlugin as an extension, a
 * TorrentPluginFactory should be instantiated with this function
 * as a parameter. The TorrentPluginFactory object should then be
 * passed at add_extension method call.
 */
boost::shared_ptr<torrent_plugin> create_video_plugin(torrent* t, void* params);

} /* namespace btstream */

#endif /* VIDEOTORRENTPLUGIN_H_ */
