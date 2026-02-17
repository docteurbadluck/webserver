#include "RulesInitialiser.hpp"
#include "IConfigReader.hpp"

void	RulesInitialiser::init_server_rules(t_server_rules &serverRules)
{
	t_server_config	config;

	config = this->reader.get_parsed_config();
	pass_data_from_config_to_rules(config, serverRules);
}

void	RulesInitialiser::pass_data_from_config_to_rules(
			const t_server_config &config, t_server_rules &server_rules)
{
	server_rules.enable_directory_listing = config.enable_directory_listing;
	server_rules.enable_upload = config.enable_upload;
	server_rules.backlog = config.backlog;
	server_rules.ip_port_vector = config.ip_port_vector;
	server_rules.error_page_filepath = config.error_page_filepath;
	server_rules.max_size_request_body = config.max_size_request_body;
	server_rules.list_allowed_method = config.list_allowed_method;
	server_rules.redirection_permanent = config.redirection_permanent;
	server_rules.redirection_temporaire = config.redirection_tmp;
	server_rules.file_system_root = config.file_system_root;
	server_rules.upload_folder = config.upload_folder;
	server_rules.default_filepath = config.default_filepath;
	server_rules.CGI_pathfile = config.CGI_pathfile;
	server_rules.client_timeout_in_second = config.client_timeout_in_second;
	server_rules.session_timeout_in_minute = config.session_timeout_in_minute;
	server_rules.session_cookie_name = config.session_cookie_name;
	server_rules.session_secure_only = config.session_secure_only;
	server_rules.max_sessions = config.max_sessions;
	server_rules.restrained_filepath = config.restrained_filepath;
}

RulesInitialiser::RulesInitialiser(IConfigReader &config_reader) : reader(config_reader)
{

}

