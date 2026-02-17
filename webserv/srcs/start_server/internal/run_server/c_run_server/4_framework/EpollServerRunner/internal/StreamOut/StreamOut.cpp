#include "StreamOut.hpp"
#include "IClient.hpp"
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>


streamout_status StreamOut::pump(IClient *client)
{
    StreamState &s = client->stream;

    if (!s.active)
		return STREAMOUT_DONE;

	streamout_status buf_status = send_buffer(client, s);
    if (buf_status == STREAMOUT_ERROR)
		return STREAMOUT_CLOSE;
    if (buf_status == STREAMOUT_CONTINUE)
		 return STREAMOUT_CONTINUE;

	streamout_status file_status = send_file(client, s);

    if (file_status == STREAMOUT_ERROR)
        return STREAMOUT_CLOSE;
    if (file_status == STREAMOUT_CONTINUE)
        return STREAMOUT_CONTINUE;

    bool should_close = s.close_after;
    s.active = false;
    s.offset = 0;
    s.buffer.clear();
    s.fd_offset = 0;
    if (s.fd != -1)
        close(s.fd);
    s.fd = -1;

    if (should_close)
        return STREAMOUT_CLOSE;
    if (!client->pending_answers.empty())
    {
        prepare_next_response(client);
        return STREAMOUT_CONTINUE;
    }

    return STREAMOUT_DONE;
}

streamout_status StreamOut::send_buffer(IClient *client, StreamState &s)
{
    if (s.offset >= s.buffer.size())
        return STREAMOUT_DONE;

    ssize_t sent = send(client->get_fd(), s.buffer.data() + s.offset,
                        s.buffer.size() - s.offset, 0);
    if (sent < 0)
    {
        return STREAMOUT_ERROR; 
    }

    s.offset += sent;
    return (s.offset >= s.buffer.size()) ? STREAMOUT_DONE : STREAMOUT_CONTINUE;
}

streamout_status StreamOut::send_file(IClient *client, StreamState &s)
{
    if (s.fd == -1)
        return STREAMOUT_DONE;

    char buf[4096];
    // lit depuis le fichier en avançant le fd lui-même
    ssize_t r = read(s.fd, buf, sizeof(buf)); // simple read

    if (r < 0)
    {
        return STREAMOUT_ERROR; // erreur fatale
    }

    if (r == 0)
    {
        close(s.fd);
        s.fd = -1;
        return STREAMOUT_DONE; // fin de fichier
    }

    ssize_t sent = send(client->get_fd(), buf, r, 0);
    if (sent < 0)
    {
        return STREAMOUT_ERROR; // erreur fatale sur le socket
    }

    s.fd_offset += sent; // pour garder trace du total envoyé si besoin
    return STREAMOUT_CONTINUE;
}

/*
streamout_status StreamOut::send_file(IClient *client, StreamState &s)
{
    if (s.fd == -1)
        return STREAMOUT_DONE;

    char buf[4096];
    ssize_t r = pread(s.fd, buf, sizeof(buf), s.fd_offset); // probabaly forbiden also 

    if (r < 0)
    {
        return STREAMOUT_ERROR;
    }

    if (r == 0)
    {
        close(s.fd);
        s.fd = -1;
        return STREAMOUT_DONE;
    }

    ssize_t sent = send(client->get_fd(), buf, r, 0);
    if (sent < 0)
    {
        return STREAMOUT_ERROR;
    }

    s.fd_offset += sent;
    return STREAMOUT_CONTINUE;
}*/

void StreamOut::prepare_next_response(IClient *client)
{
    answer_from_server_s ans = client->pending_answers.front();
    client->pending_answers.pop_front();

    StreamState &s = client->stream;
    s.buffer = ans.header;
    s.offset = 0;
    s.fd = ans.body_fd;
    s.fd_offset = 0;
    s.close_after = ans.close_flag;
    s.active = true;
}

