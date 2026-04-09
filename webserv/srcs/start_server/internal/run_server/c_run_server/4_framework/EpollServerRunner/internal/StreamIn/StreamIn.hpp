#pragma once
#include <string>
#include <vector>
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
    void            set_max_body_size(std::size_t size);

private:
    std::size_t max_body_size;

    bool advance_one_step(IClient &client, stream_status_e &last_status);

    bool extract_header(IClient &client);
    void consume_header(IClient &client, std::string &overflow, std::size_t pos);
    bool setup_chunked_request(IClient &client);
    bool setup_content_length_request(IClient &client);
    std::size_t extract_content_length(const std::string &headers) const;

    bool extract_body(IClient &client);
    bool extract_chunked_body(IClient &client);
    bool has_chunked_encoding(const std::string &headers) const;
    bool parse_chunk_size(const std::string &overflow, std::size_t pos,
                          std::size_t &chunk_size, std::size_t &data_start);
    bool append_chunk_data(const std::string &overflow, std::size_t data_start,
                           std::size_t chunk_size, std::vector<char> &decoded,
                           std::size_t &pos);
    bool finalize_chunked_body(IClient &client, const std::vector<char> &decoded,
                               std::string &overflow, std::size_t end_pos);
    bool skip_trailers(const std::string &overflow, std::size_t final_crlf,
                       std::size_t &end_pos);
};
