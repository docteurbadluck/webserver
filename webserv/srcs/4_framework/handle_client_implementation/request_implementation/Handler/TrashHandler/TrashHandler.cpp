#include "TrashHandler.hpp"

std::string TrashHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	int status_code = 501;
	return build_http_response(status_code, "", server_rules.error_page_filepath, "");
}

void TrashHandler::verify_mandatory_field(const t_parsed_request &req)
{
	(void)req;
}

std::string TrashHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	(void)filepath;
	std::ostringstream buffer;
	std::string response_body;

	std::ifstream file(error_page_filepath.c_str());
	if (file)
	{
		std::ostringstream file_content;
		file_content << file.rdbuf();
		response_body = file_content.str();
	}
	buffer << "HTTP/1.1 501 Not Implemented\r\n";
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	buffer << "\r\n"; 
	buffer << response_body;
	return buffer.str();
}

