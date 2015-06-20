#BTStream
BTStream is an open-source and cross-platform environment designed to allow scalable multimedia streaming using the BitTorrent protocol.

It consists of three tools:

  * *libbtstream* - a C++ library that implements a BitTorrent client and provides a simple API to read downloaded pieces sequentially, allowing the development of P2P multimedia streaming software. It implements different piece selection algorithms and allows the implementation of new BitTorrent extensions.

  * *BTStreamSrc* - a [GStreamer](http://gstreamer.freedesktop.org/) plugin that uses libbtstream to define a source element that can be used to easily build multimedia streaming applications with the [GStreamer](http://gstreamer.freedesktop.org/) framework.

  * *BTStreamClient* - a simple P2P VoD client written in Python that can be used to watch video files distributed in BitTorrent swarms. The client is developed with [GStreamer](http://gstreamer.freedesktop.org/) and uses the BTStreamSrc plugin. Also, it can be used for experimental evaluation of BitTorrent extensions, providing statistics like download and upload rate, time to start playback and number of playback interruptions.

All three tools are compatible with existing BitTorrent software and allow streaming of video files distributed in ordinary swarms.

##Support

Check our Wiki for support on installing and using BTStream tools:

  * [Installing](https://github.com/gabrielgmendonca/btstream/wiki/Installing)
  * [Usage](https://github.com/gabrielgmendonca/btstream/wiki/Usage)
  * [API Documentation](https://rawgit.com/gabrielgmendonca/btstream/master/libbtstream/doc/html/classbtstream_1_1_b_t_stream.html)

Additional support can be obtained at our mailing list: http://groups.google.com/group/btstream-users.

##License

All BTStream tools are under the [GNU Lesser General Public License](http://www.gnu.org/copyleft/lesser.html).

##Publications

  * Mendonça, G. G. and Leão, R. M. M. [BTStream – Um ambiente para desenvolvimento e teste de aplicações de streaming P2P](https://www.land.ufrj.br/laboratory/repository/upfiles/inproceedings/BTStream_SBRC.pdf). In _Proc. of the XXX Simpósio Brasileiro de Redes de Computadores e Sistemas Distribuídos_, Ouro Preto, MG, Brazil. May, 2012. *(in Portuguese)*
  * Mendonça, G. and Leão, R.M. M. [Projeto e análise de um sistema de nano caches residenciais para distribuição de vídeo](http://sbrc2015.ufes.br/wp-content/uploads/138604.1.pdf). In _Proc. of the XXXIII Simpósio Brasileiro de Redes de Computadores e Sistemas Distribuídos_, Vitória, ES, Brazil. May, 2015. *(in Portuguese)*
