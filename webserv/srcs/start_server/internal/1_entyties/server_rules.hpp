#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>

#define PASSWORD "thank_you"


typedef struct s_server_rules
{
	bool 												enable_directory_listing;
	bool 												enable_upload;
	int													backlog;
	int 												max_size_request_body;
	int 												client_timeout_in_second;
	int													session_timeout_in_minute;
	std::string											session_cookie_name;
	bool												session_secure_only;
	int													max_sessions;

	std::vector <std::pair<unsigned long, int> >		ip_port_vector;
	std::vector<std::string> 							list_allowed_method;
	std::vector<std::string>							CGI_pathfile;
	std::vector<std::pair<std::string, std::string> >	redirection_permanent;
	std::vector<std::pair<std::string, std::string> >	redirection_temporaire;
	std::pair<std::string, std::string> 				file_system_root;
	std::string 										error_page_filepath;
	std::string 										upload_folder;
	std::string 										default_filepath;
	std::map<std::string, std::set<std::string> >		restrained_filepath;
}	t_server_rules;
