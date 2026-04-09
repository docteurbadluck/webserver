#include "StreamIn.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>

static const std::size_t MAX_OVERFLOW_SIZE = 1073741824 + 8192; // 1GB body + 8KB headers
static const std::size_t MAX_HEADER_SIZE = 8192;

stream_status_e StreamIn::read_from_fd(int fd, IClient &client)
{
    char buffer[4096];

    ssize_t bytes = read(fd, buffer, sizeof(buffer));
    if (bytes == 0)
        return STREAM_DISCONNECT;
    if (bytes < 0)
    {
        return STREAM_ERROR;
    }

    if (client.overflow.size() + bytes > MAX_OVERFLOW_SIZE)
        return STREAM_ERROR;
    client.overflow.append(buffer, bytes);
    return STREAM_NEED_MORE_DATA;
}

stream_status_e StreamIn::parse_from_overflow(IClient &client)
{
    stream_status_e last_status = STREAM_NEED_MORE_DATA;
    while (advance_one_step(client, last_status))
        ;
    return last_status;
}

bool StreamIn::advance_one_step(IClient &client, stream_status_e &last_status)
{
    if (!client.current.header_complete)
        return extract_header(client);
    if (!client.current.body_complete)
        return extract_body(client);
    client.raw_request_completed.push_back(client.current);
    client.current = raw_request_s();
    last_status = STREAM_BODY_COMPLETE;
    return true;
}


void StreamIn::set_max_body_size(std::size_t size)
{
    max_body_size = size;
}
