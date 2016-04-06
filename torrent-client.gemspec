Gem::Specification.new do |s|
  s.name    = 'torrent-client'
  s.version = '0.1.0'
  s.summary = 'A simple Ruby torrent downloader'
  s.author  = 'Michael Smith'
  s.email   = 'mikael.t.smith@gmail.com'
  s.homepage = 'https://github.com/MikaelSmith/torrent-client'
  s.license = 'Apache 2.0'

  s.files = %w[README.md CHANGELOG.md] + Dir.glob("ext/**/*.{cc,rb}")

  s.extensions << 'ext/torrent_client/extconf.rb'

  s.add_development_dependency 'rake-compiler'
end
