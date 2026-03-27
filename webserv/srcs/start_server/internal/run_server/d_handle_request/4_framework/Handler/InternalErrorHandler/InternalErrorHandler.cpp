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

std::string InternalErrorHandler::build_http_response(
        int status_code,
        const std::string &body,
        const std::string &error_page_filepath,
        const std::string &filepath)
{
    (void)status_code;
    std::string response_body = body.empty() ? load_error_page(error_page_filepath) : body;
    return build_standard_response(500, response_body, filepath, false, true);
}
