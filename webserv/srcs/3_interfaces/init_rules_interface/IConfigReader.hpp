#pragma once
#include "usecases.hpp"

class	IConfigReader
{
	public :
	virtual t_server_config	get_parsed_config() = 0;
	virtual ~IConfigReader() {};
};
