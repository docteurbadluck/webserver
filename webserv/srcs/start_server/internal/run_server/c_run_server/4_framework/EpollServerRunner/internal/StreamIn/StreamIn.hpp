#pragma once
#include <string>
#include "IClient.hpp"
#include <sys/socket.h>
#include <errno.h>

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

private:
    bool extract_header(IClient &client);
    bool extract_body(IClient &client);
	std::size_t extract_content_length(const std::string &headers) const;

};

/*
enum stream_status_e {
    STREAM_OK,
    STREAM_HEADER_COMPLETE,
	STREAM_BODY_COMPLETE,
    STREAM_DISCONNECT
};

//the mission of this object is to : accumulate the request into client;
// this object is stateless, it annalyse the data structure in client 
struct raw_request_s;

class StreamIn
{
	public :
	    stream_status_e read_from_fd(int fd, raw_request_s &req);
	private:
    stream_status_e accumulate_headers(const char *buffer, std::size_t len, raw_request_s &req);
    stream_status_e accumulate_body(const char *buffer, std::size_t len, raw_request_s &req);
	std::size_t extract_content_length(const std::string &headers) const;

};
*/
