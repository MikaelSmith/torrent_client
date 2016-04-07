# Changelog

## [0.2.1] - 2016-04-06

Fixes TorrentClient.verify, which wasn't returning false when the file was
corrupted. Also should be more robust to halting on errors.

## [0.2.0] - 2016-04-06

Adds TorrentClient.verify, moves libtorrent session creation to happen once when
the module is loaded, and adds instructions in the README.

## [0.1.0] - 2016-04-05

Initial release, supporting TorrentClient.download.

