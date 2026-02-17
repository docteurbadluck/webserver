#include "DeleteHandler.hpp"

std::string DeleteHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);
	
	std::string filepath = build_file_path(url_path, server_rules); 

	int status_code = check_file_status(filepath, server_rules);
	if (status_code == 200)
		delete_file(filepath, status_code);
	return build_http_response(status_code, "", server_rules.error_page_filepath, filepath);
}


void DeleteHandler::delete_file(const std::string &filepath, int &status_code)
{
	if (std::remove(filepath.c_str()) != 0)
	{
		status_code = 500;
	}
}

std::string DeleteHandler::build_file_path(const std::string &path, const t_server_rules &server_rules)
{
	std::string file_path = server_rules.file_system_root.second;

	if (!file_path.empty() && file_path[file_path.size() - 1] != '/')
	    file_path += '/';
	std::string relative_path = path;
	if (!server_rules.file_system_root.first.empty())
	{
		if (relative_path.find(server_rules.file_system_root.first) == 0)
			relative_path = relative_path.substr(server_rules.file_system_root.first.size());
		else
			return "";
	}
	if (!relative_path.empty() && relative_path[0] == '/')
		relative_path = relative_path.substr(1);  
	file_path += relative_path;
	struct stat st;
	if (stat(file_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode) && file_path[file_path.size() - 1] != '/')
	{
		file_path += "/";
	}
	return file_path;
}

int DeleteHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
    struct stat st;
    (void)server_rules;

    // Vérifie si le fichier existe
    if (stat(file_path.c_str(), &st) != 0)
        return 404;

    // Vérifie si c'est un répertoire
    if (S_ISDIR(st.st_mode))
        return 403;

    // Vérifie si le parent est accessible en écriture (droit de suppression)
    std::string parent = file_path.substr(0, file_path.find_last_of('/'));
    if (access(parent.c_str(), W_OK | X_OK) != 0)
        return 403;

    // Tout est ok
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
	std::ostringstream buffer;
	std::string response_body;
	if (status_code != 200 && status_code != 204 && !error_page_filepath.empty())
	{
		std::ifstream file(error_page_filepath.c_str());
		if (file) 
		{
			std::ostringstream file_content; file_content << file.rdbuf();
			response_body = file_content.str();
		}
   	}
	if (status_code == 204)
	{
		buffer << this->http_version + " 204 No Content\r\n";
	}
	else if (status_code == 404)
	{
		buffer << this->http_version + " 404 Not Found\r\n";
	}
	else if (status_code == 403)
	{
		buffer << this->http_version + " 403 Forbidden\r\n";
	}
	else if (status_code == 500)
	{
		buffer << this->http_version + " 500 Internal Server Error\r\n";
	}
	else
	{
		buffer << this->http_version + " 200 OK\r\n";
		if (response_body.empty()) response_body = "<html><body><h1>File deleted successfully</h1></body></html>";
	}
	buffer << handler_connection_type();
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	if (this->set_cookie_flag == false)
	{
		buffer << "Set-Cookie: session_id=" << this->session_handler->generate_session_id() << "; Path=/\r\n";
	}
	buffer << "\r\n";
	if (status_code != 204)
		buffer << response_body;
	close_flag = find_out_close_flag(buffer.str());
	return buffer.str();
}
