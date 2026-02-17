#include "Client.hpp"

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

