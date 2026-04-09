#include "EpollServerRunner.hpp"

static answer_from_server_s make_payload_too_large();


void EpollServerRunner::handle_requests(IClient *client)
{
    process_completed_requests(client);
    reset_client_stream(client);
    this->mode_switcher->switch_to_write_mode(client);
}

void EpollServerRunner::process_completed_requests(IClient *client)
{
    for (size_t i = 0; i < client->raw_request_completed.size(); ++i)
    {
        raw_request_s &req = client->raw_request_completed[i];
        if (req.body_too_large)
            client->pending_answers.push_back(make_payload_too_large());
        else
            client->pending_answers.push_back(build_answer(req));
    }
}

answer_from_server_s EpollServerRunner::build_answer(raw_request_s &req)
{
    std::string body(req.raw_body.begin(), req.raw_body.end());
    answer_from_server_s ans;
    ans.header     = request_handler.handle_the_request(server_rules, session_handler, req.headers, body);
    ans.close_flag = request_handler.get_close_flag();
    ans.body_fd    = request_handler.get_fd_stream();
    ans.cgi_pid    = request_handler.get_cgi_pid();
    return ans;
}

void EpollServerRunner::reset_client_stream(IClient *client)
{
    client->stream.offset    = 0;
    client->stream.fd_offset = 0;
    client->raw_request_completed.clear();
}

static answer_from_server_s make_payload_too_large()
{
    answer_from_server_s ans;
    ans.header = "HTTP/1.1 413 Payload Too Large\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
    ans.close_flag = 1;
    return ans;
}