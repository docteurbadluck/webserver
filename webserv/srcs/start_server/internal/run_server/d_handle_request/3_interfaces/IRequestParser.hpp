#pragma once
#include <string>
#include "parsed_request.hpp"

class IRequestParser
{
	public :
		virtual t_parsed_request	parse_txt_request() = 0;
		virtual	void				init_request_txt(const std::string &req) = 0;
		virtual ~IRequestParser() {};
};