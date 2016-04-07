#include <ruby.h>
#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>

namespace lt = libtorrent;

VALUE TorrentClient = Qnil;
std::auto_ptr<lt::session> _session;

extern "C" {
    VALUE method_torrentclient_download(VALUE self, VALUE torrent_file, VALUE destination_dir);
    VALUE method_torrentclient_verify(VALUE self, VALUE torrent_file, VALUE destination_dir);

    void Init_torrent_client()
    {
        TorrentClient = rb_define_module("TorrentClient");
        rb_define_singleton_method(TorrentClient, "download", RUBY_METHOD_FUNC(method_torrentclient_download), 2);
        rb_define_singleton_method(TorrentClient, "verify", RUBY_METHOD_FUNC(method_torrentclient_verify), 2);

        try {
            _session.reset(new lt::session());
            _session->set_alert_mask(lt::alert::status_notification | lt::alert::error_notification);

            lt::error_code ec;
            _session->listen_on(std::make_pair(6881, 6889), ec);
            if (ec) {
                std::string err = "failed to open listen socket: " + ec.message();
                rb_raise(rb_eRuntimeError, err.c_str());
            }
        } catch (std::exception &e) {
            rb_raise(rb_eRuntimeError, "failed from unknown exception");
        }
    }

    VALUE method_torrentclient_download(VALUE self, VALUE torrent_file, VALUE destination_dir)
    {
        try {
            lt::error_code ec;
            lt::add_torrent_params p;
            p.save_path = StringValueCStr(destination_dir);
            p.ti = new lt::torrent_info(StringValueCStr(torrent_file), ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            lt::torrent_handle handle = _session->add_torrent(p, ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            while (true) {
                if (_session->wait_for_alert(lt::seconds(1))) {
                    std::auto_ptr<lt::alert> a = _session->pop_alert();
                    if (a->type() == lt::torrent_finished_alert::alert_type) {
                        break;
                    }
                }
            }
            _session->remove_torrent(handle);
            return Qtrue;
        } catch (std::exception &e) {
            rb_raise(rb_eRuntimeError, "failed from unknown exception");
        }
        return Qnil;
    }

    VALUE method_torrentclient_verify(VALUE self, VALUE torrent_file, VALUE destination_dir)
    {
        try {
            lt::error_code ec;
            lt::add_torrent_params p;
            p.save_path = StringValueCStr(destination_dir);
            p.flags = lt::add_torrent_params::flag_paused;
            p.ti = new lt::torrent_info(StringValueCStr(torrent_file), ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            lt::torrent_handle handle = _session->add_torrent(p, ec);
            if (ec) {
                rb_raise(rb_eRuntimeError, ec.message().c_str());
                return Qnil;
            }

            lt::torrent_status status = handle.status();
            while (status.state == lt::torrent_status::checking_files ||
                   status.state == lt::torrent_status::checking_resume_data) {
                _session->wait_for_alert(lt::seconds(1));
                status = handle.status();
            }

            _session->remove_torrent(handle);

            if (status.state == lt::torrent_status::seeding ||
                status.state == lt::torrent_status::queued_for_checking) {
                return Qtrue;
            } else {
                return Qfalse;
            }
        } catch (std::exception &e) {
            rb_raise(rb_eRuntimeError, "failed from unknown exception");
        }
        return Qnil;
    }
}
