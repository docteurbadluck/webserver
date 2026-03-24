#pragma once
#include <string>
#include "IClient.hpp"
#include <sys/socket.h>

enum stream_status_e
{
    STREAM_NEED_MORE_DATA,
    STREAM_HEADER_COMPLETE,
    STREAM_BODY_COMPLETE,
    STREAM_DISCONNECT,
    STREAM_ERROR
};

class StreamIn
{
public:
    stream_status_e read_from_fd(int fd, IClient &client);
	stream_status_e parse_from_overflow(IClient &client);
    void set_max_body_size(std::size_t size);

private:
    std::size_t max_body_size;
    bool extract_header(IClient &client);
    bool extract_body(IClient &client);
    bool extract_chunked_body(IClient &client);
	std::size_t extract_content_length(const std::string &headers) const;
    bool has_chunked_encoding(const std::string &headers) const;
};
