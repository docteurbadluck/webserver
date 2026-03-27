#include "PostUploadHandler.hpp"

std::string PostUploadHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	this->set_cookie_flag = this->request.were_cookies_sent;
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);

	std::string filepath = build_file_path_simple(url_path, server_rules);

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
		return path;
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
	return true;
}

int PostUploadHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
    struct stat st;

    if (stat(file_path.c_str(), &st) == 0)
    {
        if (access(file_path.c_str(), W_OK) != 0)
            return 403;
        if (S_ISDIR(st.st_mode))
            return 403;
        if (this->request.body.size() > static_cast<size_t>(server_rules.max_size_request_body))
            return 413;
        return 200;
    }

    std::string parent_dir = file_path.substr(0, file_path.find_last_of('/'));

    if (parent_dir.empty() || access(parent_dir.c_str(), F_OK) != 0)
        return 404;
    if (access(parent_dir.c_str(), W_OK) != 0)
        return 403;

    if (this->request.body.size() > static_cast<size_t>(server_rules.max_size_request_body))
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
	std::string response_body;

	if (status_code != 200 && status_code != 204)
		response_body = load_error_page(error_page_filepath);
	if (status_code == 200 && response_body.empty())
		response_body = "<html><body><h1>File uploaded successfully</h1></body></html>";

	return build_standard_response(status_code, response_body, filepath, true, false);
}
