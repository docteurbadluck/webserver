#include "RequestParser.hpp"
#include <iostream>

RequestParser::RequestParser()
{
	this->parsed_request.id_request = -1;
	this->parsed_request.http_version = -1;
	this->parsed_request.Connection = -1;
	this->parsed_request.body_lenght = -1;
	this->parsed_request.were_cookies_sent = false;
	this->parsed_request.turn_off_server = false;
	this->init_header_field_map();
}

RequestParser::~RequestParser()
{
}

void	RequestParser::init_request_txt(const std::string &req)
{
	this->request_txt = req;
}
