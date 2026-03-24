#include "StreamIn.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>

static const std::size_t MAX_OVERFLOW_SIZE = 1073741824 + 8192; // 1GB body + 8KB headers
static const std::size_t MAX_HEADER_SIZE = 8192;

std::size_t StreamIn::extract_content_length(const std::string &headers) const
{
    const std::string key = "Content-Length:";
    std::size_t pos = headers.find(key);
    if (pos == std::string::npos)
        return 0;

    pos += key.size();
    while (pos < headers.size() && headers[pos] == ' ')
        ++pos;

    std::istringstream iss(headers.c_str() + pos);
    std::size_t val = 0;
    iss >> val;
    return val;
}

bool StreamIn::has_chunked_encoding(const std::string &headers) const
{
    std::string lower_headers = headers;
    for (std::size_t i = 0; i < lower_headers.size(); ++i)
    {
        if (lower_headers[i] >= 'A' && lower_headers[i] <= 'Z')
            lower_headers[i] = lower_headers[i] + ('a' - 'A');
    }
    return lower_headers.find("transfer-encoding: chunked") != std::string::npos
        || lower_headers.find("transfer-encoding:chunked") != std::string::npos;
}

void StreamIn::set_max_body_size(std::size_t size)
{
    max_body_size = size;
}

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
            client.current = raw_request_s();
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
    {
        if (overflow.size() > MAX_HEADER_SIZE)
            return false;
        return false;
    }
    if (pos > MAX_HEADER_SIZE)
        return false;

    client.current.headers = overflow.substr(0, pos + 4);
    overflow.erase(0, pos + 4);

    client.current.header_complete = true;

    if (has_chunked_encoding(client.current.headers))
    {
        client.current.is_chunked = true;
        client.current.expected_body_size = 0;
        return true;
    }

    ssize_t cl = extract_content_length(client.current.headers);
    if (cl < 0)
        return false;

    if (max_body_size > 0 && static_cast<std::size_t>(cl) > max_body_size)
    {
        client.current.expected_body_size = cl;
        client.current.body_complete = true;
        client.current.body_too_large = true;
        return true;
    }

    client.current.expected_body_size = cl;

    if (cl == 0)
        client.current.body_complete = true;

    return true;
}

bool StreamIn::extract_body(IClient &client)
{
    if (client.current.is_chunked)
        return extract_chunked_body(client);

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

bool StreamIn::extract_chunked_body(IClient &client)
{
    std::string &overflow = client.overflow;
    std::vector<char> decoded;

    std::size_t pos = 0;
    while (pos < overflow.size())
    {
        // Find end of chunk-size line
        std::size_t crlf = overflow.find("\r\n", pos);
        if (crlf == std::string::npos)
            return false; // need more data

        // Parse hex chunk size
        std::string size_str = overflow.substr(pos, crlf - pos);
        // Strip chunk extensions (after semicolon)
        std::size_t semi = size_str.find(';');
        if (semi != std::string::npos)
            size_str = size_str.substr(0, semi);

        std::istringstream iss(size_str);
        std::size_t chunk_size = 0;
        iss >> std::hex >> chunk_size;

        if (chunk_size == 0)
        {
            // Final chunk: expect trailing \r\n after 0\r\n
            std::size_t end_pos = crlf + 2;
            // Skip optional trailers until empty line
            std::size_t final_crlf = overflow.find("\r\n", end_pos);
            if (final_crlf == std::string::npos)
                return false; // need more data
            // If there are trailers, skip them; if immediate \r\n, that's the end
            if (final_crlf == end_pos)
                end_pos = final_crlf + 2;
            else
            {
                // Skip trailer lines until \r\n\r\n
                while (true)
                {
                    std::size_t next = overflow.find("\r\n", final_crlf + 2);
                    if (next == std::string::npos)
                        return false;
                    if (next == final_crlf + 2)
                    {
                        end_pos = next + 2;
                        break;
                    }
                    final_crlf = next;
                }
            }

            client.current.raw_body = decoded;
            client.current.expected_body_size = decoded.size();
            overflow.erase(0, end_pos);
            client.current.body_complete = true;
            return true;
        }

        // Need chunk data + trailing \r\n
        std::size_t data_start = crlf + 2;
        if (overflow.size() < data_start + chunk_size + 2)
            return false; // need more data

        // Append chunk data
        decoded.insert(decoded.end(),
                       overflow.begin() + data_start,
                       overflow.begin() + data_start + chunk_size);

        // Move past chunk data + \r\n
        pos = data_start + chunk_size + 2;
    }

    return false; // need more data
}
