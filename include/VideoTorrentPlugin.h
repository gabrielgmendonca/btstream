/*
 * Copyright 2011 Gabriel Mendonça
 *
 * This file is part of BiVoD.
 * BiVoD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BiVoD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BiVoD.  If not, see <http://www.gnu.org/licenses/>.
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

using namespace libtorrent;

namespace bivod {

class VideoTorrentPlugin: public torrent_plugin {
public:
	VideoTorrentPlugin(torrent* t);

	/**
	 * Returns a VideoPeerPlugin.
	 * Called when a new peer is connected to the torrent.
	 */
	virtual boost::shared_ptr<peer_plugin> new_connection(peer_connection* pc);

	/**
	 * Informs torrent that the received piece will be read.
	 * Called when a piece is received and pass the hash check.
	 */
	virtual void on_piece_pass(int index);

private:
	torrent* m_torrent;
};

/**
 * Function template for a torrent_plugin factory function.
 */
typedef boost::function<boost::shared_ptr<libtorrent::torrent_plugin> (libtorrent::torrent *, void *)> TorrentPluginFactory;

/**
 * Returns a new VideoTorrentPlugin.
 * In order to add VideoTorrentPlugin as an extension, a
 * TorrentPluginFactory should be instantiated with this function
 * as a parameter. The TorrentPluginFactory object should then be
 * passed at add_extension method call.
 */
boost::shared_ptr<torrent_plugin> create_video_plugin(torrent* t, void* params);

} /* namespace bivod */

#endif /* VIDEOTORRENTPLUGIN_H_ */
