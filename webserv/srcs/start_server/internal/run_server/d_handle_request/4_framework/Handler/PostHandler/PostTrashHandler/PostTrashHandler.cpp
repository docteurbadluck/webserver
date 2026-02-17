#include "PostTrashHandler.hpp"

std::string PostTrashHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	int status_code = 405;
	return build_http_response(status_code, request.body, server_rules.error_page_filepath, "");
}

void PostTrashHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "POST")
		throw std::runtime_error("POST: Invalid method, expected POST");
}


std::string PostTrashHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	(void)filepath;
	(void)error_page_filepath;
	(void) status_code;
	std::ostringstream	buffer;
	std::string			response_body;
	std::ifstream		file(error_page_filepath.c_str());
	if (file)
	{
		std::ostringstream file_content;
		file_content << file.rdbuf();
		response_body = file_content.str();
	}
	buffer << this->http_version + " 405 Methode Not Allowed \r\n";
	buffer << handler_connection_type();
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << "text/html" << "\r\n";
	buffer << "\r\n"; 
	buffer << response_body;
    close_flag = find_out_close_flag(buffer.str());
	return buffer.str();
}
