# torrent_client

A simple Ruby BitTorrent client based on libtorrent-rasterbar

## Setup

Requires [Boost](https://boost.org) and [libtorrent](http://libtorrent.org).
On Mac OS X these can be setup with Homebrew
```
brew install boost libtorrent-rasterbar
```

## Usage

```
# Returns true if the torrent payload is complete on-disk, false otherwise.
TorrentClient.verify '<file>.torrent' '<destination_dir>'

# Downloads the torrent payload to the destination_dir. Returns when complete.
TorrentClient.download '<file>.torrent' '<destination_dir>'
```
