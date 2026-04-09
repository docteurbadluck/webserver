#include "StreamOut.hpp"
#include "IClient.hpp"
#include <unistd.h>
#include <ctime>
#include <signal.h>
#include <sys/wait.h>

static void reset_stream(StreamState &s);

streamout_status StreamOut::finalize_stream(IClient *client, StreamState &s)
{
    bool should_close = s.close_after;
    reap_cgi(s);
    reset_stream(s);
    if (should_close)
        return STREAMOUT_CLOSE;
    if (!client->pending_answers.empty())
    {
        prepare_next_response(client);
        return STREAMOUT_CONTINUE;
    }
    return STREAMOUT_DONE;
}


void StreamOut::reap_cgi(StreamState &s)
{
    if (s.cgi_pid <= 0)
        return;
    kill(s.cgi_pid, SIGKILL);
    waitpid(s.cgi_pid, NULL, 0);
    s.cgi_pid = -1;
}


static void reset_stream(StreamState &s)
{
    s.active = false;
    s.offset = 0;
    s.buffer.clear();
    s.fd_offset = 0;
    if (s.fd != -1)
        close(s.fd);
    s.fd = -1;
}

void StreamOut::prepare_next_response(IClient *client)
{
    answer_from_server_s ans = client->pending_answers.front();
    client->pending_answers.pop_front();
    StreamState &s   = client->stream;
    s.buffer         = ans.header;
    s.offset         = 0;
    s.fd             = ans.body_fd;
    s.fd_offset      = 0;
    s.close_after    = ans.close_flag;
    s.cgi_pid        = ans.cgi_pid;
    s.cgi_start_time = (ans.cgi_pid > 0) ? std::time(NULL) : 0;
    s.active         = true;
}
