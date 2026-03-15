#include "EpollServerRunner.hpp"

void EpollServerRunner::handle_requests(IClient *client)
{
    for (size_t i = 0; i < client->raw_request_completed.size(); ++i)
    {
        raw_request_s &req = client->raw_request_completed[i];

        std::string body(
            req.raw_body.begin(),
            req.raw_body.end());

        answer_from_server_s ans;
        ans.header = request_handler.handle_the_request(
            server_rules,
            session_handler,
            req.headers,
            body
        );

        ans.close_flag = request_handler.get_close_flag();
        ans.body_fd    = request_handler.get_fd_stream();
        ans.cgi_pid    = request_handler.get_cgi_pid();

        client->pending_answers.push_back(ans);
    }
	client->stream.offset = 0;
	client->stream.fd_offset = 0;
    client->raw_request_completed.clear();
	this->mode_switcher->switch_to_write_mode(client);
}

