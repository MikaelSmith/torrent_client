#include <ruby.h>
#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>

VALUE TorrentClient = Qnil;

extern "C" {
    VALUE method_torrentclient_download(VALUE self, VALUE torrent_file, VALUE destination_dir);

    void Init_torrent_client()
    {
        TorrentClient = rb_define_module("TorrentClient");
        rb_define_singleton_method(TorrentClient, "download", RUBY_METHOD_FUNC(method_torrentclient_download), 2);
    }

    VALUE method_torrentclient_download(VALUE self, VALUE torrent_file, VALUE destination_dir)
    {
        try {
            namespace lt = libtorrent;

            lt::session s;
            lt::error_code ec;
            s.listen_on(std::make_pair(6881, 6889), ec);
            if (ec) {
                std::string err = "failed to open listen socket: " + ec.message();
                rb_raise(rb_eRuntimeError, err.c_str());
                return Qnil;
            }

            lt::add_torrent_params p;
            p.save_path = StringValueCStr(destination_dir);
            p.ti = new lt::torrent_info(StringValueCStr(torrent_file), ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            s.add_torrent(p, ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            while (true) {
                if (s.wait_for_alert(lt::seconds(1))) {
                    std::auto_ptr<lt::alert> a = s.pop_alert();
                    if (a->type() == lt::torrent_finished_alert::alert_type) {
                        return Qtrue;
                    }
                }
            }
        } catch (std::exception &e) {
            rb_raise(rb_eRuntimeError, "failed from unknown exception");
        }
        return Qnil;
    }
}
