#include "StreamOut.hpp"
#include "IClient.hpp"
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctime>
#include <unistd.h>

static const int CGI_TIMEOUT_SECONDS = 5;


streamout_status StreamOut::send_file(IClient *client, StreamState &s)
{
    if (s.fd == -1)
        return STREAMOUT_DONE;
    if (is_cgi_timed_out(s))
        return timeout_cgi(s);
    char buf[4096];
    ssize_t r = read(s.fd, buf, sizeof(buf));
    if (r < 0)
        return STREAMOUT_CONTINUE;
    if (r == 0)
        return finalize_fd(s);
    return send_fd_chunk(client, s, buf, r);
}

bool StreamOut::is_cgi_timed_out(StreamState &s)
{
    return s.cgi_pid > 0 && s.cgi_start_time > 0
        && (std::time(NULL) - s.cgi_start_time > CGI_TIMEOUT_SECONDS);
}

streamout_status StreamOut::timeout_cgi(StreamState &s)
{
    reap_cgi(s);
    close(s.fd);
    s.fd = -1;
    return STREAMOUT_ERROR;
}


streamout_status StreamOut::finalize_fd(StreamState &s)
{
    close(s.fd);
    s.fd = -1;
    reap_cgi(s);
    return STREAMOUT_DONE;
}

streamout_status StreamOut::send_fd_chunk(IClient *client, StreamState &s, char *buf, ssize_t r)
{
    ssize_t sent = send(client->get_fd(), buf, r, 0);
    if (sent < 0)
        return STREAMOUT_ERROR;
    s.fd_offset += sent;
    if (sent < r)
    {
        s.buffer.assign(buf + sent, r - sent);
        s.offset = 0;
    }
    return STREAMOUT_CONTINUE;
}
