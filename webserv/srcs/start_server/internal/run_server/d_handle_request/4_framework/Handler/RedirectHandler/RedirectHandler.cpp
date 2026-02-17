
#include "RedirectHandler.hpp"
#include <sstream>
#include <fstream>

std::string RedirectHandler::handle(
    const t_parsed_request &req,
    const t_server_rules &server_rules,
    SessionHandlerUC &session_handler)
{
    verify_mandatory_field(req);
    init_field(req, server_rules, session_handler);

    const std::string &path = req.path;
    std::string resp;

    resp = check_redirection(
        path,
        server_rules.redirection_permanent,
        301,
        server_rules.error_page_filepath
    );
    if (!resp.empty())
        return resp;
    resp = check_redirection(
        path,
        server_rules.redirection_temporaire,
        302,
        server_rules.error_page_filepath
    );
    if (!resp.empty())
        return resp;
    return build_http_response(
        404,
        "",
        "",
        server_rules.error_page_filepath
    );
}

void RedirectHandler::verify_mandatory_field(const t_parsed_request &req)
{
    (void)req;
}

std::string RedirectHandler::build_http_response(
    int status_code,
    const std::string &body,
    const std::string &file_path,
    const std::string &error_page_filepath)
{
    std::ostringstream buffer;
   std::string response_body;
	(void)body;
    /* ---------- STATUS LINE ---------- */
    if (status_code == 301)
        buffer << this->http_version +" 301 Moved Permanently\r\n";
    else if (status_code == 302)
        buffer << this->http_version + " 302 Found\r\n";
    else if (status_code == 404)
        buffer << this->http_version +" 404 Not Found\r\n";

    /* ---------- REDIRECTION ---------- */
    if (status_code == 301 || status_code == 302)
    {
        buffer << "Location: " << file_path << "\r\n";
        buffer << "Content-Length: 0\r\n";
		buffer<< handler_connection_type();	
        buffer << "\r\n";
        return buffer.str();
    }

    /* ---------- ERROR PAGE ---------- */
    std::ifstream file(error_page_filepath.c_str());
    if (file.is_open())
    {
        std::ostringstream tmp;
        tmp << file.rdbuf();
        response_body = tmp.str();
    }
	buffer << handler_connection_type();
    buffer << "Content-Type: text/html\r\n";
    buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "\r\n";
    buffer << response_body;
    close_flag = find_out_close_flag(buffer.str());
    return buffer.str();
}

static bool starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size()
        && str.compare(0, prefix.size(), prefix) == 0;
}

std::string RedirectHandler::check_redirection(
    const std::string &path,
    const std::vector<std::pair<std::string, std::string> > &redirections,
    int status_code,
    const std::string &error_page_filepath)
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
            redirections.begin();
         it != redirections.end();
         ++it)
    {
        const std::string &old_path = it->first;
        const std::string &new_path = it->second;

        if (starts_with(path, old_path))
        {
            std::string suffix = path.substr(old_path.size());
            std::string redirected_path = new_path + suffix;

            return build_http_response(
                status_code,
                "",
                redirected_path,
                error_page_filepath
            );
        }
    }

    return "";
}






















