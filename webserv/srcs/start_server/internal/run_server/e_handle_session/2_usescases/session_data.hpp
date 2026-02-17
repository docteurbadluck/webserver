#pragma once
#include "usecases.hpp"
#include <ctime>

typedef struct s_session_data
{
	std::string	session_id;
	std::string	username;
	std::time_t	creation_time;
	std::time_t	last_activity;
	bool		is_authentificated;
}	t_session_data;
