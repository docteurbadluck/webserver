#include "Client.hpp"

void Client::appendToReadBuffer(const std::string &data)
{
	this->read_buffer.append(data);
}
const std::string & Client::getReadBuffer() const
{
	return this->read_buffer;
}

bool Client::delimiter_found_in_readbuffer()
{
	this->pos_delimiter = this->read_buffer.find(this->delimiter);
	return (this->pos_delimiter != std::string::npos);
}

void	Client::update_last_activity()
{
	this->last_activity = std::time(NULL);
}

bool	Client::is_timed_out(std::time_t now, int timeout)
{
	if (now > timeout + this->last_activity)
		return true;
	return false;
}
