#include "InternalErrorHandler.hpp"
#include <string>

std::string InternalErrorHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	
	return build_http_response(500, "", server_rules.error_page_filepath, "");
}

void InternalErrorHandler::verify_mandatory_field(const t_parsed_request &req)
{
	(void)req;
}


std::string InternalErrorHandler::build_http_response
(
        int status_code,
        const std::string &body,
        const std::string &error_page_filepath,
        const std::string &filepath)
{
    std::ostringstream buffer;
    std::string response_body;
    const std::string *content_src = &body;

    if (body.empty() && !error_page_filepath.empty())
    {
        std::ifstream file(error_page_filepath.c_str());
        if (file)
        {
            std::ostringstream tmp;
            tmp << file.rdbuf();
            response_body = tmp.str();
            content_src = &response_body;
		}
	}

    buffer << this->http_version + " 500 Internal Server Error\r\n";
	buffer << handler_connection_type();
    buffer << "Content-Length: " << content_src->size() << "\r\n";
    buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
    buffer << "Connection: close\r\n";
    buffer << "\r\n";
    buffer << *content_src;
    close_flag = find_out_close_flag(buffer.str());

    return buffer.str();
}



































