#include "CGIHandler.hpp"

std::string CGIHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	verify_mandatory_field(req);
	init_field(req, server_rules, session_handler);
	std::string url_path = map_to_url_path(req.path, server_rules.file_system_root);
	std::string filepath = build_file_path_simple(url_path, server_rules);

	// Check if this file has a CGI extension (derived from CGI_pathfile entries)
	int flag = 0;
	size_t dot_pos = filepath.find_last_of('.');
	if (dot_pos != std::string::npos)
	{
		std::string file_ext = filepath.substr(dot_pos);
		for (std::vector<std::string>::const_iterator i = server_rules.CGI_pathfile.begin();
			i != server_rules.CGI_pathfile.end(); ++i)
		{
			size_t cfg_dot = i->find_last_of('.');
			if (cfg_dot != std::string::npos && i->substr(cfg_dot) == file_ext)
				flag = 1;
		}
	}
	if (!flag)
		return "";

	int status_code = check_file_status(filepath, server_rules);
	if (status_code != 200)
	{
		this->set_cookie_flag = this->request.were_cookies_sent;
		return build_http_response(status_code, "", server_rules.error_page_filepath, filepath);
	}

	// Execute the CGI script
	int ret = execute_cgi(filepath, req, server_rules);
	if (ret == -1)
	{
		this->set_cookie_flag = this->request.were_cookies_sent;
		return build_http_response(500, "", server_rules.error_page_filepath, filepath);
	}

	// Build headers only — body comes via fd_stream (the pipe)
	this->set_cookie_flag = this->request.were_cookies_sent;
	return build_http_response(200, "", server_rules.error_page_filepath, filepath);
}

int CGIHandler::execute_cgi(const std::string &filepath, const t_parsed_request &req, const t_server_rules &server_rules)
{
	int stdout_pipe[2];
	int stdin_pipe[2];
	bool has_body = (req.methode == "POST" && req.body.size() > 0);

	// Create pipe for CGI stdout
	if (pipe(stdout_pipe) == -1)
		return -1;

	// Create pipe for CGI stdin (POST body)
	if (has_body)
	{
		if (pipe(stdin_pipe) == -1)
		{
			close(stdout_pipe[0]);
			close(stdout_pipe[1]);
			return -1;
		}
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		close(stdout_pipe[0]);
		close(stdout_pipe[1]);
		if (has_body)
		{
			close(stdin_pipe[0]);
			close(stdin_pipe[1]);
		}
		return -1;
	}

	if (pid == 0)
	{
		// === CHILD PROCESS ===

		// Redirect stdout to pipe
		close(stdout_pipe[0]);
		dup2(stdout_pipe[1], STDOUT_FILENO);
		close(stdout_pipe[1]);

		// Redirect stdin from pipe (for POST body)
		if (has_body)
		{
			close(stdin_pipe[1]);
			dup2(stdin_pipe[0], STDIN_FILENO);
			close(stdin_pipe[0]);
		}

		// chdir to script directory for relative path access
		std::string dir = filepath.substr(0, filepath.find_last_of('/'));
		if (!dir.empty())
			chdir(dir.c_str());

		// Setup environment variables
		setup_env(filepath, req, server_rules);
		char **envp = build_envp();

		// Build argv: just the script path
		char *argv[2];
		argv[0] = const_cast<char*>(filepath.c_str());
		argv[1] = NULL;

		// execve — Linux handles shebang automatically
		execve(filepath.c_str(), argv, envp);

		// If execve fails, clean up and exit
		delete[] envp;
		_exit(1);
	}

	// === PARENT PROCESS ===

	// Close write end of stdout pipe (we only read)
	close(stdout_pipe[1]);

	// Write POST body to child's stdin, then close
	if (has_body)
	{
		close(stdin_pipe[0]);
		write(stdin_pipe[1], req.body.c_str(), req.body.size());
		close(stdin_pipe[1]);
	}

	// Set pipe read-end to non-blocking for epoll
	fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);

	// Store the pipe fd and child PID for StreamOut to read from and reap
	this->fd_stream = stdout_pipe[0];
	this->cgi_pid = pid;

	return 0;
}

void CGIHandler::setup_env(const std::string &filepath, const t_parsed_request &req, const t_server_rules &server_rules)
{
	env_vars.clear();

	env_vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env_vars.push_back("REQUEST_METHOD=" + req.methode);
	env_vars.push_back("SCRIPT_FILENAME=" + filepath);
	env_vars.push_back("REDIRECT_STATUS=200");

	// Extract query string from path (everything after '?')
	std::string query_string;
	std::string path_info = req.path;
	size_t qpos = req.path.find('?');
	if (qpos != std::string::npos)
	{
		query_string = req.path.substr(qpos + 1);
		path_info = req.path.substr(0, qpos);
	}
	env_vars.push_back("QUERY_STRING=" + query_string);
	env_vars.push_back("PATH_INFO=" + path_info);

	// Content info (for POST)
	std::ostringstream cl;
	cl << req.body_lenght;
	env_vars.push_back("CONTENT_LENGTH=" + cl.str());
	env_vars.push_back("CONTENT_TYPE=" + req.content_type);

	// Server info
	env_vars.push_back("SERVER_NAME=" + req.host);
	if (!server_rules.ip_port_vector.empty())
	{
		std::ostringstream port;
		port << server_rules.ip_port_vector[0].second;
		env_vars.push_back("SERVER_PORT=" + port.str());
	}
	if (req.http_version == 1)
		env_vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	else
		env_vars.push_back("SERVER_PROTOCOL=HTTP/1.0");

	// Cookies
	if (!req.cookies.empty())
	{
		std::string cookie_str;
		for (size_t i = 0; i < req.cookies.size(); ++i)
		{
			if (i > 0)
				cookie_str += "; ";
			cookie_str += req.cookies[i];
		}
		env_vars.push_back("HTTP_COOKIE=" + cookie_str);
	}
}

char** CGIHandler::build_envp()
{
	char **envp = new char*[env_vars.size() + 1];
	for (size_t i = 0; i < env_vars.size(); ++i)
		envp[i] = const_cast<char*>(env_vars[i].c_str());
	envp[env_vars.size()] = NULL;
	return envp;
}

int CGIHandler::check_file_status(const std::string &file_path, const t_server_rules &server_rules)
{
	struct stat st;
	(void)server_rules;

	if (stat(file_path.c_str(), &st) != 0)
		return 404;

	if (S_ISDIR(st.st_mode))
		return 403;

	// Check execute permission for CGI
	if (access(file_path.c_str(), X_OK) != 0)
		return 403;

	return 200;
}

void CGIHandler::verify_mandatory_field(const t_parsed_request &req)
{
	(void)req;
}

std::string CGIHandler::build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath)
{
	(void)body;
	std::ostringstream buffer;

	buffer << status_line(status_code);

	// For CGI 200: Connection: close (unknown content length), no body in header
	// The CGI script output (via fd_stream) provides the remaining headers + body
	if (status_code == 200)
	{
		buffer << "Connection: close\r\n";
		if (this->set_cookie_flag == false)
			buffer << "Set-Cookie: session_id=" << this->session_handler->generate_session_id() << "; Path=/\r\n";
		close_flag = 1;
		return buffer.str();
	}

	// For errors: send full response with body
	std::ostringstream code_str;
	code_str << status_code;
	std::string response_body = load_error_page(error_page_filepath);
	response_body = replace_all_occurrences(response_body, "{{statusCode}}", code_str.str());
	response_body = replace_all_occurrences(response_body, "{{reasonPhrase}}", reason_phrase(status_code));
	buffer << handler_connection_type();
	buffer << "Content-Length: " << response_body.size() << "\r\n";
	buffer << "Content-Type: " << get_mime_type(filepath, status_code) << "\r\n";
	if (this->set_cookie_flag == false)
		buffer << "Set-Cookie: session_id=" << this->session_handler->generate_session_id() << "; Path=/\r\n";
	buffer << "\r\n";
	buffer << response_body;
	close_flag = find_out_close_flag(buffer.str());
	return buffer.str();
}
