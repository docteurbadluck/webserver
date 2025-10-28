#include "Client.hpp"

void 	Client::extract_header()
{
	this->request = this->read_buffer.substr(0, this->pos_delimiter);
}

void Client::flush_header()
{
	this->read_buffer.erase(0, this->pos_delimiter + this->delimiter.size());
}

std::string Client::extract_body(int body_lenght)
{
	std::string	body;

	if(this->read_buffer.size() >= static_cast<long unsigned int>(body_lenght))
		body = this->read_buffer.substr(0, body_lenght );
	return (body);
}

void Client::flush_body(int body_lenght)
{
	this->read_buffer.erase(0, body_lenght);
}
