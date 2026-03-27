#pragma once
#include <string>
#include <ctime>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <deque>

struct StreamState
{
    std::string buffer;
    size_t      offset;
    int         fd;
	bool		active;
	size_t		fd_offset;
	bool		close_after;
	pid_t		cgi_pid;
	std::time_t	cgi_start_time;
	StreamState(): fd(-1), close_after(false), cgi_pid(-1), cgi_start_time(0){}
};

struct raw_request_s
{
    std::string headers;
    std::vector<char> raw_body;

    bool        header_complete;
    bool        body_complete;
    std::size_t expected_body_size;
    bool        is_chunked;
    bool        body_too_large;

    raw_request_s(): header_complete(false), body_complete(false), expected_body_size(0), is_chunked(false), body_too_large(false){}
};


struct answer_from_server_s
{
    std::string header;
    int         body_fd;
	int			close_flag;
	pid_t		cgi_pid;
    answer_from_server_s(): body_fd(-1), cgi_pid(-1){}
};
    
class IClient
{
	public :
	StreamState							stream; 

	std::string							overflow;
	std::vector<raw_request_s>			raw_request_completed;
	raw_request_s						current;

	//answer_from_server_s				answer_from_server;
	std::deque<answer_from_server_s>	pending_answers;

	std::time_t					last_activity;

	virtual ~IClient() {};
	virtual int		get_fd() const = 0;

   void reset_raw_request()
    {
		this->current.headers.clear();
		this->current.raw_body.clear();
		this->current.header_complete = false;
		this->current.body_complete = false;
		this->current.expected_body_size = 0;
		this->current.is_chunked = false;
    }
};
