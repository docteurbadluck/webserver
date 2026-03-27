#include "DeleteHandler.hpp"

std::string DeleteHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);

	std::string filepath = build_file_path_simple(url_path, server_rules);

	int status_code = check_file_status(filepath, server_rules);
	if (status_code == 200)
		delete_file(filepath, status_code);
	return build_http_response(status_code, "", server_rules.error_page_filepath, filepath);
}

void DeleteHandler::delete_file(const std::string &filepath, int &status_code)
{
	if (std::remove(filepath.c_str()) != 0)
		status_code = 500;
}

int DeleteHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
    struct stat st;
    (void)server_rules;

    if (stat(file_path.c_str(), &st) != 0)
        return 404;
    if (S_ISDIR(st.st_mode))
        return 403;

    std::string parent = file_path.substr(0, file_path.find_last_of('/'));
    if (access(parent.c_str(), W_OK | X_OK) != 0)
        return 403;

    return 200;
}

void DeleteHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "DELETE")
		throw std::runtime_error("DELETE: Invalid method, expected GET");
	if (req.path.empty())
		throw std::runtime_error("DELETE: Missing request target");
}

std::string DeleteHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	std::string response_body;

	if (status_code != 200 && status_code != 204)
		response_body = load_error_page(error_page_filepath);
	if (status_code == 200 && response_body.empty())
		response_body = "<html><body><h1>File deleted successfully</h1></body></html>";

	return build_standard_response(status_code, response_body, filepath, true, false);
}
