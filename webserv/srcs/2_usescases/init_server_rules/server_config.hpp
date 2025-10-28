#pragma once 
#include <string>
#include <vector>

typedef struct s_server_config
{
	int	backlog;
	int	client_timeout_in_second;
	int session_timeout_in_minute;
	std::string session_cookie_name;
	bool session_secure_only;
	int max_sessions;
	std::vector <std::pair<unsigned long, int> > ip_port_vector;	
	std::string error_page_filepath;
	int max_size_request_body;
	std::vector<std::string> list_allowed_method;
	std::vector<std::pair<std::string, std::string> >redirection;
	std::pair<std::string, std::string> file_system_root;
	bool enable_directory_listing;
	bool enable_upload;
	std::string upload_folder;
	std::string default_filepath;
	std::vector<std::string> CGI_pathfile;
}	t_server_config;


