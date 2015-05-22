## libbtstream and BTStreamSrc ##

Both libbtstream and BTStreamSrc can be compiled and installed with GNU Autotools. First, you will need to install `Automake` and `Autoconf`. On popular Linux distributions like Ubuntu, Fedora and Debian, these can be installed through the package `build-essential`.

After that, you just need to follow the standard install procedure for Unix software:

```
tar xvzf libbtstream-X.X.tar.gz (or tar xvzf btstreamsrc-X.X.tar.gz)
cd libbtstream-X.X.tar.gz (or cd btstreamsrc-X.X.tar.gz)
./configure
make
sudo make install
```

The `configure` script will check the dependencies. If it fails to find any dependency, install it and try again. The libbtstream tool depends on [libtorrent-rasterbar](http://www.rasterbar.com/products/libtorrent/) and [Boost libraries](http://www.boost.org). BTStreamSrc depends on [GStreamer](http://www.gstreamer.net/).

On Ubuntu, for example, all dependencies can be installed with the command

```
sudo apt-get install build-essential libtorrent-rasterbar-dev libgstreamer0.10-dev
```

After installing the BTStreamSrc plugin, you can test it with the command
```
gst-inspect-0.10 btstreamsrc
```

If GStreamer founds the plugin, its documentation will be shown on the console.

## BTStreamClient ##

The BTStreamClient doesn't need to be compiled and can be directly executed with Python. You just need to extract the tar.gz file:
```
tar xvzf btstreamclient-X.X.tar.gz
cd btstreamclient-X.X.tar.gz
python btstreamclient.py --help
```

For usage instructions, see the [Wiki page](http://code.google.com/p/btstream/wiki/Usage).