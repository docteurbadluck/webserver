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
	this->cgi_pid = -1;
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

pid_t BaseHandler::get_cgi_pid()
{
	return cgi_pid;
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

std::string BaseHandler::load_error_page(const std::string &error_page_filepath)
{
	if (error_page_filepath.empty())
		return "";
	std::ifstream file(error_page_filepath.c_str());
	if (!file)
		return "";
	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

std::string BaseHandler::status_line(int status_code)
{
	std::string v = this->http_version + " ";
	switch (status_code)
	{
		case 200: return v + "200 OK\r\n";
		case 204: return v + "204 No Content\r\n";
		case 301: return v + "301 Moved Permanently\r\n";
		case 302: return v + "302 Found\r\n";
		case 400: return v + "400 Bad Request\r\n";
		case 403: return v + "403 Forbidden\r\n";
		case 404: return v + "404 Not Found\r\n";
		case 405: return v + "405 Method Not Allowed\r\n";
		case 408: return v + "408 Request Timeout\r\n";
		case 413: return v + "413 Payload Too Large\r\n";
		case 500: return v + "500 Internal Server Error\r\n";
		case 501: return v + "501 Not Implemented\r\n";
		default:
		{
			std::ostringstream oss;
			oss << v << status_code << " Unknown\r\n";
			return oss.str();
		}
	}
}

std::string BaseHandler::reason_phrase(int status_code)
{
	switch (status_code)
	{
		case 200: return "OK";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 413: return "Payload Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		default: return "Unknown";
	}
}

std::string BaseHandler::replace_all_occurrences(const std::string &src, const std::string &from, const std::string &to)
{
	std::string result = src;
	std::size_t pos = 0;
	while ((pos = result.find(from, pos)) != std::string::npos)
	{
		result.replace(pos, from.size(), to);
		pos += to.size();
	}
	return result;
}

std::string BaseHandler::build_standard_response(int status_code,
	const std::string &response_body,
	const std::string &filepath,
	bool include_cookie,
	bool force_close)
{
	std::string body = response_body;
	std::ostringstream code_str;
	code_str << status_code;
	body = replace_all_occurrences(body, "{{statusCode}}", code_str.str());
	body = replace_all_occurrences(body, "{{reasonPhrase}}", reason_phrase(status_code));

	std::ostringstream buffer;
	buffer << status_line(status_code);
	buffer << handler_connection_type();
	buffer << "Content-Length: " << body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	if (force_close)
		buffer << "Connection: close\r\n";
	if (include_cookie && !this->set_cookie_flag)
		buffer << "Set-Cookie: session_id=" << this->session_handler->generate_session_id() << "; Path=/\r\n";
	buffer << "\r\n";
	if (status_code != 204)
		buffer << body;
	close_flag = find_out_close_flag(buffer.str());
	return buffer.str();
}

std::string BaseHandler::build_file_path_simple(const std::string &path,
	const t_server_rules &server_rules)
{
	std::string file_path;
	std::string relative_path;

	if (!manage_file_system_root(path, server_rules, file_path, relative_path))
		return "";
	return normalize_directory(file_path);
}
