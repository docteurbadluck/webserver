#include "MethodNotAllowedHandler.hpp"
#include <string>

std::string get_parent_directory(const std::string &path)
{
    // cas path vide → parent vide
    if (path.empty())
        return "";

    std::string tmp = path;

    // retirer éventuel slash de fin (ex: "/a/b/")
    while (tmp.size() > 1 && tmp[tmp.size() - 1] == '/')
        tmp.erase(tmp.size() - 1);

    // trouver dernier slash
    std::string::size_type pos = tmp.find_last_of('/');
    if (pos == std::string::npos)
        return ""; // pas de parent

    if (pos == 0)
        return "/"; // parent de "/a" → "/"

    return tmp.substr(0, pos);
}

bool filesystem_exists(const std::string &path)
{
    struct stat st;

    if (stat(path.c_str(), &st) == 0)
        return true;
    return false;
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


std::string MethodNotAllowedHandler::build_http_response
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
    if (status_code == 404)
        buffer << this->http_version + " 404 Not Found\r\n";
    else if (status_code == 403)
        buffer << this->http_version + " 403 Forbidden\r\n";
    else if (status_code == 405)
        buffer << this->http_version + " 405 Method Not Allowed\r\n";
    else if (status_code == 501)
        buffer << this->http_version + " 501 Not Implemented\r\n";
    else
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



































