#include "MethodNotAllowedHandler.hpp"
#include <string>

std::string get_parent_directory(const std::string &path)
{
    if (path.empty())
        return "";

    std::string tmp = path;

    while (tmp.size() > 1 && tmp[tmp.size() - 1] == '/')
        tmp.erase(tmp.size() - 1);

    std::string::size_type pos = tmp.find_last_of('/');
    if (pos == std::string::npos)
        return "";

    if (pos == 0)
        return "/";

    return tmp.substr(0, pos);
}

bool filesystem_exists(const std::string &path)
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}

std::string MethodNotAllowedHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
    const std::string &path = req.path;

    if (req.methode == "GET" || req.methode == "DELETE")
    {
		std::string url_path = map_to_url_path(path, server_rules.file_system_root);
		std::string file_path = build_file_path(url_path, server_rules);

		int res = check_file_status(file_path, server_rules);
        if (res == 404)
            return build_http_response(res, "", server_rules.error_page_filepath, "");
		else
			return build_http_response(405, "", server_rules.error_page_filepath, "");
    }
    if (req.methode == "POST")
    {
        std::string parent = get_parent_directory(path);
        if (!filesystem_exists(parent))
            return build_http_response(404, "", server_rules.error_page_filepath, "");
    }
    return build_http_response(501, "", server_rules.error_page_filepath, "");
}

void MethodNotAllowedHandler::verify_mandatory_field(const t_parsed_request &req)
{
	(void)req;
}

std::string MethodNotAllowedHandler::build_http_response(
        int status_code,
        const std::string &body,
        const std::string &error_page_filepath,
        const std::string &filepath)
{
    std::string response_body = body.empty() ? load_error_page(error_page_filepath) : body;
    return build_standard_response(status_code, response_body, filepath, false, true);
}
