#include "RequestHandlerUC.hpp"
#include "IRequestParser.hpp"
#include "IRequestHandler.hpp"
#include "ISessionPolicy.hpp"
#include "parsed_request.hpp"
#include <ctime>   
#include <iostream>

RequestHandlerUC::RequestHandlerUC(IRequestParser &request_parser, IRequestHandler &request_handler) : parser(request_parser), handler(request_handler)
{

}

RequestHandlerUC::~RequestHandlerUC()
{
}

std::string RequestHandlerUC::handle_the_request(const t_server_rules &server_rules,
													SessionHandlerUC &session_handler,
													const std::string &body)
{
	std::string			answer;
	t_parsed_request	new_request;

	new_request = this->parser.parse_txt_request();
	if (body.size() > 0)
		new_request.body = body;
	session_handler.add_session_from_request(new_request);
	answer = this->handler.handle(new_request, server_rules, session_handler);
	this->file_content = this->handler.get_file_content();
	return (answer);
}

void	RequestHandlerUC::init_request_txt(const std::string &req )
{
	this->parser.init_request_txt(req);
}

t_parsed_request		RequestHandlerUC::parse_header()
{
	return (this->parser.parse_txt_request());
}
