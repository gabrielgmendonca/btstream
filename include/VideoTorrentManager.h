/*
 * VideoTorrentManager.h
 *
 *  Created on: 06/09/2011
 *      Author: gabriel
 */

#ifndef VIDEOTORRENTMANAGER_H_
#define VIDEOTORRENTMANAGER_H_

#include <libtorrent/session.hpp>

using namespace libtorrent;

namespace bivod {

class VideoTorrentManager {
public:
	VideoTorrentManager();

private:
	session m_session;
};

} /* namespace bivod */

#endif /* VIDEOTORRENTMANAGER_H_ */
