#include "EpollServerRunner.hpp"

/*
void    EpollServerRunner::handle_request(IClient *client)
{
    std::string answer;

    if (client->stream.active)
	{
		return;
	}
	std::string body(
		client->raw_request.raw_body.begin(),
		client->raw_request.raw_body.end()
	);

    answer = this->request_handler.handle_the_request(
						this->server_rules,
						this->session_handler,
						client->raw_request.headers,
						body);

    //put the answer in client 
    client->answer_from_server.header = answer;
    client->answer_from_server.close_flag = this->request_handler.get_close_flag();
	client->answer_from_server.body_fd = this->request_handler.get_fd_stream();

	client->stream.buffer = client->answer_from_server.header;
    client->stream.fd = client->answer_from_server.body_fd;
    client->stream.offset = 0;
	
	this->timeout_manager.update_timestamp_last_activity(client);
	this->mode_switcher->switch_to_write_mode(client);
	client->reset_raw_request();
}
*/


void EpollServerRunner::handle_requests(IClient *client)
{
    for (size_t i = 0; i < client->raw_request_completed.size(); ++i)
    {
        raw_request_s &req = client->raw_request_completed[i];

        std::string body(
            req.raw_body.begin(),
            req.raw_body.end()
        );

        answer_from_server_s ans;
        ans.header = request_handler.handle_the_request(
            server_rules,
            session_handler,
            req.headers,
            body
        );

        ans.close_flag = request_handler.get_close_flag();
        ans.body_fd    = request_handler.get_fd_stream();

        client->pending_answers.push_back(ans);
    }
	client->stream.offset = 0;
	client->stream.fd_offset = 0;
    client->raw_request_completed.clear();
	this->mode_switcher->switch_to_write_mode(client);
}

