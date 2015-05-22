## libbtstream ##

The `btstream::BTStream` class defines `libbtstream`'s API. It's description can be found at `libbtstream`'s [Doxygen documentation](http://btstream.googlecode.com/git/libbtstream/doc/html/classbtstream_1_1_b_t_stream.html).

Here is an example of a simple video streaming client written in C++ that shows how the library can be used:

```
#include <string>
#include <libbtstream/btstream.h>

int main(int argc, char* argv[]) {
    if (argc == 2) {
        // Read torrent file path from command line.
        std::string torrent_path = argv[1];

        // Initialize BTStream and start download with BitTorrent.
        // A path to a torrent file and a piece selection algorithm
        // are given. The constructor has many optional parameters.
        btstream::BTStream bt(torrent_path, btstream::SEQUENTIAL);

        // Wait until the first piece is downloaded.
        boost::shared_ptr<btstream::Piece> piece;
        piece = bt.get_next_piece();

        // Receive all torrent pieces and play them sequentially.
        while (piece) {
            // Video playback code enters here.
            my_func(piece->data, piece->size);

            // Get pointer to next piece.
            piece = bt.get_next_piece();
        }
    }

    return 0;
}
```

The library provides `pkg-config` support so that library paths, include paths and link flags can be automatically added with the command `pkg-config --libs --cflags libbtstream`. Using `pkg-config`, the program can be compiled with:

```
g++ -Wall my_video_client.cpp -o my_video_client `pkg-config --libs --cflags libbtstream`
```

## BTStreamSrc ##

With GStreamer and `BTStreamSrc`, a P2P media streaming client can be easily written in any language supported by GStreamer like C, C++, Python and Java. Here we provide a sample audio streaming client written in Python:

```
import sys
import gst
import gtk

class Main:
    def __init__(self):
        if len(sys.argv) == 2:
            # Read torrent file path from command line.
            torrent_path = sys.argv[1]

            # Create GStreamer pipeline.
            self.pipeline = gst.Pipeline("music-player")

            # Create and add BTStreamSrc element.
            # Set torrent file path and piece selection algorithm.
            self.src = gst.element_factory_make("btstreamsrc", "src")

            self.src.set_property("torrent", torrent_path)
            self.src.set_property("algorithm", "sequential")

            self.pipeline.add(self.src)

            # Create and add decoder element.
            self.decoder = gst.element_factory_make("decodebin2", "decoder")
            self.pipeline.add(self.decoder)

            # Create and add sink element.
            self.sink = gst.element_factory_make("autoaudiosink", "sink")
            self.pipeline.add(self.sink)

            # Link elements.
            self.src.link(self.decoder)
            self.decoder.link(self.sink)

            # Start playback.
            self.pipeline.set_state(gst.STATE_PLAYING)

start = Main()
gtk.main()
```

More details about `BTStreamSrc`'s API, including description of each of the element's properties, can be obtained with the `gst-inspect` tool:

```
gst-inspect-0.10 btstreamsrc
```

To learn more about GStreamer development, we recommend the [Application Development Manual](http://gstreamer.freedesktop.org/data/doc/gstreamer/head/manual/html/index.html) and the [Python GStreamer Tutorial](http://pygstdocs.berlios.de/pygst-tutorial/index.html).

## BTStreamClient ##

BTStreamClient can be launched by running the `btstreamclient.py` script on a console using Python. The client provides many command-line options. To see them use the -h or --help options.

Example:
```
python btstreamclient.py --algorithm sequential --buffer_size 10 big_buck_bunny.torrent 
```