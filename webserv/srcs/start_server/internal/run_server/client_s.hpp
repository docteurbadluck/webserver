// stream_in
struct raw_request_s
{
    std::string headers;
    std::vector<char> raw_body;

    bool        header_complete;
    bool        body_complete;
    std::size_t expected_body_size;

    raw_request_s(): header_complete(false), body_complete(false), expected_body_size(0){}
}

//handler response
struct answer_from_server_s
{
    std::string header;
    int         body_fd;
    std::size_t body_size;
    bool        close_flag;
    answer_from_server_s(): body_fd(-1), body_size(0), close_flag(-1){}
}

// stream out_
struct stream_out_data_s
{
    bool    active;
    int     offset;
    stream_out_data_s(): active(false), offset(0){}
}

struct client_s
{
	std::time_t				timestamp_last_activity;
	raw_request_s			raw_request;
	answer_from_server_s	answer_from_server;
	stream_out_data_s		stream_out_data;
}
}
