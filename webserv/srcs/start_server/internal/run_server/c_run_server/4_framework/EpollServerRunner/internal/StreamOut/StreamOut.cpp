#include "StreamOut.hpp"
#include "IClient.hpp"
#include <unistd.h>
#include <sys/socket.h>

streamout_status StreamOut::pump(IClient *client)
{
    StreamState &s = client->stream;
    if (!s.active)
        return STREAMOUT_DONE;
    streamout_status buf = send_buffer(client, s);

    if (buf == STREAMOUT_ERROR)
        return STREAMOUT_CLOSE;
    if (buf == STREAMOUT_CONTINUE)
        return STREAMOUT_CONTINUE;

    streamout_status file = send_file(client, s);

    if (file == STREAMOUT_ERROR)
        return STREAMOUT_CLOSE;
    if (file == STREAMOUT_CONTINUE)
        return STREAMOUT_CONTINUE;

    return finalize_stream(client, s);
}

streamout_status StreamOut::send_buffer(IClient *client, StreamState &s)
{
    if (s.offset >= s.buffer.size())
        return STREAMOUT_DONE;
    ssize_t sent = send(client->get_fd(), s.buffer.data() + s.offset,
                        s.buffer.size() - s.offset, 0);
    if (sent < 0)
        return STREAMOUT_ERROR;
    s.offset += sent;
    return (s.offset >= s.buffer.size()) ? STREAMOUT_DONE : STREAMOUT_CONTINUE;
}
