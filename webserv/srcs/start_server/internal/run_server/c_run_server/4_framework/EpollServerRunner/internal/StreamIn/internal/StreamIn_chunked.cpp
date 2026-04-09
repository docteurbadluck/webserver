#include "StreamIn.hpp"
#include <sstream>

bool StreamIn::extract_chunked_body(IClient &client)
{
    std::string &overflow = client.overflow;
    std::vector<char> decoded;
    std::size_t pos = 0;

    while (pos < overflow.size())
    {
        std::size_t chunk_size;
        std::size_t data_start;
        if (!parse_chunk_size(overflow, pos, chunk_size, data_start))
            return false;
        if (chunk_size == 0)
            return finalize_chunked_body(client, decoded, overflow, data_start);
        if (!append_chunk_data(overflow, data_start, chunk_size, decoded, pos))
            return false;
    }
    return false;
}

bool StreamIn::parse_chunk_size(const std::string &overflow, std::size_t pos,
                                 std::size_t &chunk_size, std::size_t &data_start)
{
    std::size_t crlf = overflow.find("\r\n", pos);
    if (crlf == std::string::npos)
        return false;
    std::string size_str = overflow.substr(pos, crlf - pos);
    std::size_t semi = size_str.find(';');
    if (semi != std::string::npos)
        size_str = size_str.substr(0, semi);
    std::istringstream iss(size_str);
    iss >> std::hex >> chunk_size;
    data_start = crlf + 2;
    return true;
}

bool StreamIn::finalize_chunked_body(IClient &client, const std::vector<char> &decoded,
                                      std::string &overflow, std::size_t end_pos)
{
    std::size_t final_crlf = overflow.find("\r\n", end_pos);
    if (final_crlf == std::string::npos)
        return false;
    if (final_crlf == end_pos)
        end_pos = final_crlf + 2;
    else if (!skip_trailers(overflow, final_crlf, end_pos))
        return false;
    client.current.raw_body = decoded;
    client.current.expected_body_size = decoded.size();
    overflow.erase(0, end_pos);
    client.current.body_complete = true;
    return true;
}

bool StreamIn::skip_trailers(const std::string &overflow, std::size_t final_crlf,
                              std::size_t &end_pos)
{
    while (true)
    {
        std::size_t next = overflow.find("\r\n", final_crlf + 2);
        if (next == std::string::npos)
            return false;
        if (next == final_crlf + 2)
        {
            end_pos = next + 2;
            return true;
        }
        final_crlf = next;
    }
}

bool StreamIn::append_chunk_data(const std::string &overflow, std::size_t data_start,
                                  std::size_t chunk_size, std::vector<char> &decoded,
                                  std::size_t &pos)
{
    if (overflow.size() < data_start + chunk_size + 2)
        return false;
    decoded.insert(decoded.end(),
                   overflow.begin() + data_start,
                   overflow.begin() + data_start + chunk_size);
    pos = data_start + chunk_size + 2;
    return true;
}
