#include "StreamIn.hpp"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>

static const std::size_t MAX_HEADER_SIZE = 8192;

bool StreamIn::extract_header(IClient &client)
{
    std::string &overflow = client.overflow;
    std::size_t pos = overflow.find("\r\n\r\n");
    if (pos == std::string::npos || pos > MAX_HEADER_SIZE)
        return false;
    consume_header(client, overflow, pos);
    if (has_chunked_encoding(client.current.headers))
        return setup_chunked_request(client);
    return setup_content_length_request(client);
}

void StreamIn::consume_header(IClient &client, std::string &overflow, std::size_t pos)
{
    client.current.headers = overflow.substr(0, pos + 4);
    overflow.erase(0, pos + 4);
    client.current.header_complete = true;
}

bool StreamIn::setup_chunked_request(IClient &client)
{
    client.current.is_chunked = true;
    client.current.expected_body_size = 0;
    return true;
}

bool StreamIn::setup_content_length_request(IClient &client)
{
    std::size_t cl = extract_content_length(client.current.headers);
    if (max_body_size > 0 && cl > max_body_size)
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

bool StreamIn::has_chunked_encoding(const std::string &headers) const
{
    std::string lower = headers;
    for (std::size_t i = 0; i < lower.size(); ++i)
        if (lower[i] >= 'A' && lower[i] <= 'Z')
            lower[i] = lower[i] + ('a' - 'A');
    return lower.find("transfer-encoding: chunked") != std::string::npos
        || lower.find("transfer-encoding:chunked") != std::string::npos;
}
