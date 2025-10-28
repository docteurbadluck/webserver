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

void RequestParser::init_header_field_map()
{
	header_field_map["host"] = &RequestParser::handle_host;
	header_field_map["content-length"] = &RequestParser::handle_content_length;
	header_field_map["content-type"] = &RequestParser::handle_content_type;
	header_field_map["connection"] = &RequestParser::handle_connection;
	header_field_map["cookie"] = &RequestParser::handle_cookies;
}

void RequestParser::attribute_field_from_txt()
{
	read_request_line();
	read_header_lines();
}

t_parsed_request	RequestParser::parse_txt_request()
{
	init_header_field_map();
	attribute_field_from_txt();
	return (this->parsed_request);
}

void	RequestParser::init_request_txt(const std::string &req)
{
	this->request_txt = req;
}