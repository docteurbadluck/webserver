#include "EpollServerRunner.hpp"

void	EpollServerRunner::handle_message_from_client(IClient *client)
{
	char	buffer[1024];
	ssize_t	count;
	
	count = recv(client->get_fd(), buffer, sizeof(buffer), 0);
	if (count <= 0)
	{
		disconnect_client(client);
		return ;
	}
	handle_received_data(client, buffer, count);
}

void	EpollServerRunner::handle_received_data(IClient *client, const char *buffer, ssize_t count)
{
	std::string buf(buffer, count);

	client->appendToReadBuffer(buf);
	while (client->delimiter_found_in_readbuffer() == true)
	{
		t_parsed_request	parsed_request;
		parse_request_header(parsed_request, client);

		if (parsed_request.methode == "GET" || parsed_request.body_lenght <= 0)
			handle_request_bodyless(client);
		else
		{
			if (!is_the_body_complete(parsed_request, client))
				break;
			handle_request_with_body(client, parsed_request);
			if(parsed_request.methode == "POST" && parsed_request.path == "/close" && parsed_request.body == PASSWORD)
				this->running = false;
		}
	}
}

void EpollServerRunner::parse_request_header(t_parsed_request	&parsed_request, IClient *client)
{
	client->extract_header(); 
	this->request_handler.init_request_txt(client->get_request());
	parsed_request = this->request_handler.parse_header();
}

void	EpollServerRunner::handle_request_bodyless(IClient *client)
{
	std::string answer;

	answer = this->request_handler.handle_the_request(this->server_rules, this->session_handler, "");
	std::size_t header_end = answer.find("\r\n\r\n");
	std::cout <<"size header : "<< header_end + 4 << std::endl;
	// send header
	send(client->get_fd(), answer.c_str(), header_end + 4, MSG_NOSIGNAL);
	write(1, answer.c_str(), header_end + 4);	

	//send body
	std::size_t sent_byte = send(client->get_fd(), answer.c_str() + (header_end + 4), answer.size() - (header_end + 4), MSG_NOSIGNAL);
	std::size_t to_send = 0;

	for (std::vector<std::string>::iterator it = this->request_handler.file_content.begin();
			it != this->request_handler.file_content.end(); ++it)
{
	to_send += it->size();
	if (to_send > sent_byte)
	{
		if (sent_byte - 8192 < to_send)
		{
			std::size_t pos = to_send - sent_byte;
			send(client->get_fd(), it->c_str()+ pos  , it->size() - pos, MSG_NOSIGNAL);
		}
		else 
		{
			send(client->get_fd(), it->c_str(), it->size(), MSG_NOSIGNAL);
		}
	}
}
	client->update_last_activity();
	client->flush_header();
}

bool	EpollServerRunner::is_the_body_complete(const t_parsed_request &parsed_request, IClient *client)
{
	if (client->getReadBuffer().size() < static_cast<long unsigned int> (parsed_request.body_lenght))
	{
		return (false);
	}
	return (true);
}

void	EpollServerRunner::handle_request_with_body(IClient *client, t_parsed_request &parsed_request)
{
	std::string answer;

	client->flush_header();
	parsed_request.body = client->extract_body(parsed_request.body_lenght);
	answer = this->request_handler.handle_the_request(server_rules, this->session_handler, parsed_request.body);
	send(client->get_fd(), answer.c_str(), answer.size(), MSG_NOSIGNAL);
	client->flush_body(parsed_request.body_lenght);
	client->update_last_activity();
}
