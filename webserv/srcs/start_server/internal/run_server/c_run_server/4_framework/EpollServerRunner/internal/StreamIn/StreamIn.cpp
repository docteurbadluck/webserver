#include "StreamIn.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>

#include "StreamIn.hpp"

std::size_t StreamIn::extract_content_length(const std::string &headers) const
{
    const std::string key = "Content-Length:";
    std::size_t pos = headers.find(key);
    if (pos == std::string::npos)
        return 0;

    pos += key.size();
    while (pos < headers.size() && headers[pos] == ' ')
        ++pos;

    return std::strtoul(headers.c_str() + pos, NULL, 10);
}

stream_status_e StreamIn::read_from_fd(int fd, IClient &client)
{
    char buffer[4096];

    ssize_t bytes = read(fd, buffer, sizeof(buffer));
    if (bytes == 0)
        return STREAM_DISCONNECT;
    if (bytes < 0)
    {
       // if (errno == EAGAIN || errno == EWOULDBLOCK)		// forbiden aparrently
         //   return STREAM_NEED_MORE_DATA;
        return STREAM_ERROR;
    }

    client.overflow.append(buffer, bytes);
    return STREAM_NEED_MORE_DATA;
}

stream_status_e StreamIn::parse_from_overflow(IClient &client)
{
    bool progress = true;
    stream_status_e last_status = STREAM_NEED_MORE_DATA;

    while (progress)
    {
        progress = false;

        if (!client.current.header_complete)
        {
            if (!extract_header(client))
                break;
            progress = true;
        }

        if (client.current.header_complete &&
            !client.current.body_complete)
        {
            if (!extract_body(client))
                break;
            progress = true;
        }

        if (client.current.header_complete &&
            client.current.body_complete)
        {
            client.raw_request_completed.push_back(client.current);
            client.current = raw_request_s(); // reset propre
            last_status = STREAM_BODY_COMPLETE;
            progress = true;
        }
    }
    return last_status;
}

bool StreamIn::extract_header(IClient &client)
{
    std::string &overflow = client.overflow;

    size_t pos = overflow.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;

    client.current.headers = overflow.substr(0, pos + 4);
    overflow.erase(0, pos + 4);

    client.current.header_complete = true;

    ssize_t cl = extract_content_length(client.current.headers);
    if (cl < 0)
        return false; // ou error

    client.current.expected_body_size = cl;

    if (cl == 0)
        client.current.body_complete = true;

    return true;
}

bool StreamIn::extract_body(IClient &client)
{
    std::string &overflow = client.overflow;
    size_t expected = client.current.expected_body_size;

    if (overflow.size() < expected)
        return false;

    client.current.raw_body.assign(
        overflow.begin(),
        overflow.begin() + expected
    );

    overflow.erase(0, expected);
    client.current.body_complete = true;
    return true;
}

