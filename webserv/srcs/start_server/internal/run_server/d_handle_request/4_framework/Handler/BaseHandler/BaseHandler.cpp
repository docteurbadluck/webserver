#include "BaseHandler.hpp"


BaseHandler::BaseHandler() {}

BaseHandler::~BaseHandler() {}

void	BaseHandler::init_field(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	this->request = req;
	this->server_rules = server_rules;
	this->session_handler = &session_handler;
	this->directory_listing = false;
	this->set_cookie_flag = false;
	if(this->request.http_version == 1)
	{
		this->http_version = "HTTP/1.1";
	}
	else
	{
		this->http_version = "HTTP/1.0";
	}
	this->fd_stream = -1;
	this->close_flag = 0;
}


// this fonction transform the file-tree name into the internet name
std::string BaseHandler::map_to_url_path(const std::string &path,
							const std::pair<std::string, std::string> &file_system_root)
{
	const std::string &fs_root   = file_system_root.second;
	const std::string &url_root  = file_system_root.first;

	if (path == "/")
		return url_root;

	if (path.find(fs_root) == 0)
	{
		return (url_root + path.substr(fs_root.size()));
	}
	return (path);
}

int BaseHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
	struct stat st;

	if (stat(file_path.c_str(), &st) != 0)
	{
		return (404);
	}
	if (!(st.st_mode & S_IRUSR))
	{
		return (403);
	}
	if (S_ISDIR(st.st_mode))
	{
		if (server_rules.enable_directory_listing == true)
			this->directory_listing = true;
		else
		{
			this->directory_listing = false;
			return (403);
		}
	}
	return (200);
}

std::string BaseHandler::read_file_content(const std::string &file_path)
{
	std::ifstream		file(file_path.c_str(), std::ios::binary);
	std::ostringstream	file_content_stream;

	if (!file.is_open())
		throw std::runtime_error("BaseHandler: File not found");
	file_content_stream << file.rdbuf();
	return (file_content_stream.str());
}

std::vector<std::string> BaseHandler::read_file_chunks(const std::string &file_path, std::size_t chunk_size)
{
	std::ifstream file(file_path.c_str(), std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("BaseHandler: File not found");

	std::vector<std::string>	chunks;
	std::string					buffer(chunk_size, '\0');

	while (file)
	{
		file.read(&buffer[0], chunk_size);
		std::streamsize bytes_read = file.gcount();
		buffer[bytes_read]= '\0';
		if (bytes_read > 0)
		{
			chunks.push_back(buffer.substr(0, bytes_read));
		}
	}
	return chunks;
}

std::string BaseHandler::get_mime_type(const std::string &path, int status_code)
{
	size_t dot_pos;
	std::string ext;

	if (status_code != 200)
		return "text/html";
	dot_pos = path.find_last_of(".");
	if (dot_pos == std::string::npos)
	{
		if (this->directory_listing == true)
			return "text/html";
		return "application/octet-stream";
	}
	ext = path.substr(dot_pos + 1);
	if (ext == "html" || ext == "htm") return "text/html";
	if (ext == "css") return "text/css";
	if (ext == "js") return "application/javascript";
	if (ext == "json") return "application/json";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	if (ext == "png") return "image/png";
	if (ext == "gif") return "image/gif";
	if (ext == "ico") return "image/x-icon";
	if (ext == "pdf") return "application/pdf";
	if (ext == "txt") return "text/plain";
	return "application/octet-stream";
}


std::string BaseHandler::interpret_html_body(const std::string body, SessionHandlerUC *session_handler)
{
	std::string	result = body;
	size_t		pos = result.find("@username");

	if (session_handler->is_valid_session(this->request.session_id) == false)
		return (result);
	while (pos != std::string::npos)
	{
		result.replace(pos, 9, session_handler->get_session(this->request.session_id).username);
		pos = result.find("@username", pos + session_handler->get_session(this->request.session_id).username.size());
	}
	return (result);
}

std::vector<std::string>	BaseHandler::get_file_content()
{
	return this->file_content;
}

std::string BaseHandler::handler_connection_type()
{
	if (this->http_version == "HTTP/1.0")
	{
		if (this->request.Connection == 0)
		{
			return "Connection: keep-alive\r\n";
		}
		return "Connection: close\r\n";
	}
	else
	{
		if (this->request.Connection == 1)
			return "Connection: close\r\n";
	}
	return "";
}

int BaseHandler::get_fd_stream()
{
	return fd_stream;
}



int BaseHandler::find_out_close_flag(const std::string &answer)
{
    size_t header_end = answer.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return 1;

    std::string headers = answer.substr(0, header_end);

    if (headers.find("Connection: close") != std::string::npos ||
        headers.find("Connection: Close") != std::string::npos)
        return 1;

    if (headers.find("HTTP/1.0") != std::string::npos &&
        headers.find("Connection: keep-alive") == std::string::npos &&
        headers.find("Connection: Keep-Alive") == std::string::npos)
        return 1;

    return 0;
}
