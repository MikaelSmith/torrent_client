require 'mkmf'
extension_name = 'torrent_client'
dir_config(extension_name)

$libs = append_library($libs, 'torrent-rasterbar')

create_makefile(extension_name)
