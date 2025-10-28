#include "PostUploadHandler.hpp"

std::string PostUploadHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);
	std::string redir_response = verify_redirection(url_path, server_rules);
	if (!redir_response.empty())
		return redir_response;
	std::string filepath = build_file_path(url_path, server_rules); 

	int status_code = check_file_status(filepath, server_rules);
	if (status_code == 200)
	{
		if(!upload_file(server_rules.upload_folder, get_filename_from_path(filepath)))
			status_code = 500;
	}
	return build_http_response(status_code, req.body, server_rules.error_page_filepath, filepath);
}

std::string PostUploadHandler::get_filename_from_path(const std::string &path)
{
	size_t pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return path; // pas de '/', donc path est déjà un nom de fichier
	return path.substr(pos + 1);
}

bool PostUploadHandler::upload_file(const std::string &upload_folder, const std::string &file_name)
{

	std::string filepath = upload_folder;
	if (filepath[filepath.size() - 1] != '/')
		filepath += '/';
	filepath += file_name;
	std::ofstream ofs(filepath.c_str(), std::ios::binary);
	if (!ofs)
	{
		std::cerr << "Failed to open file for writing: " << filepath << std::endl;
		return false;
	}
	ofs.write(request.body.c_str(), request.body.size());
	ofs.close();
	std::cout << "File uploaded successfully: " << filepath << std::endl;
	return true;
}

std::string PostUploadHandler::build_file_path(const std::string &path, const t_server_rules &server_rules)
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
int PostUploadHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
	(void)server_rules;

	struct stat st;
	if (stat(file_path.c_str(), &st) != 0)
	{
		std::string parent_dir = file_path.substr(0, file_path.find_last_of('/'));
		if (access(parent_dir.c_str(), W_OK) != 0)
			return 403;
		return 200;
	}
	if (access(file_path.c_str(), W_OK) != 0)
		return 403;
	if (S_ISDIR(st.st_mode))
		return 403;
	if (this->request.body.size() > static_cast<long unsigned int>(this->server_rules.max_size_request_body))
		return 413;
	return 200;
}


void PostUploadHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "POST")
		throw std::runtime_error("POST: Invalid method, expected GET");
}

std::string PostUploadHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	std::ostringstream buffer;
	std::string response_body;

	if (status_code != 200 && status_code != 204 && !error_page_filepath.empty())
	{
		std::ifstream file(error_page_filepath.c_str());
		if (file)
		{
			std::ostringstream file_content;
			file_content << file.rdbuf();
			response_body = file_content.str();
		}
	}
	if (status_code == 204) 
	{
		buffer << "HTTP/1.1 204 No Content\r\n";
	}
	else if (status_code == 403)
	{
		buffer << "HTTP/1.1 403 Forbidden\r\n";
	}
	else if (status_code == 500)
	{
		buffer << "HTTP/1.1 500 Internal Server Error\r\n";
	}
	else if (status_code == 413)
	{
		buffer << "HTTP/1.1 413 Payload Too Large\r\n";
	}
	else
	{
		buffer << "HTTP/1.1 200 OK\r\n";
		if (response_body.empty())
			response_body = "<html><body><h1>File uploaded successfully</h1></body></html>";
	}
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	if (this->set_cookie_flag == false)
	{
		buffer << "Set-Cookie: session_id=" << this->session_handler->generate_session_id() << "; Path=/\r\n";
	}
	buffer << "\r\n"; 
	if (status_code != 204)
		buffer << response_body;
	return buffer.str();
}

