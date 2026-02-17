#include "ConfigReader.hpp"

ConfigReader::ConfigReader(const std::string &config_filepath) :IConfigReader()
{
	FileIO	file(config_filepath);
	config_content = file.read();	
	init_config_field();
	init_new_config();
}

ConfigReader::~ConfigReader()
{
}

void ConfigReader::init_new_config()
{
	this->new_config.backlog = 0;
	this->new_config.max_size_request_body = 0;
	this->new_config.enable_directory_listing = false;
	this->new_config.enable_upload = false;
	this->new_config.client_timeout_in_second = 10;
	this->new_config.session_timeout_in_minute = 30;
	this->new_config.session_cookie_name = "session_id";
	this->new_config.session_secure_only = false;
	this->new_config.max_sessions = 1000;
	this->new_config.default_filepath = "index.html";
}

void ConfigReader::init_config_field()
{
	this->config_field.push_back(std::make_pair("backlog", &ConfigReader::init_backlog));
	this->config_field.push_back(std::make_pair("ip", &ConfigReader::init_ip));
	this->config_field.push_back(std::make_pair("port", &ConfigReader::init_port));
	this->config_field.push_back(std::make_pair("error_page_filepath", &ConfigReader::init_error_page_filepath));
	this->config_field.push_back(std::make_pair("max_size_request_body", &ConfigReader::init_max_size_request_body));
	this->config_field.push_back(std::make_pair("list_allowed_method", &ConfigReader::init_list_allowed_method));
	this->config_field.push_back(std::make_pair("redirection_tmp", &ConfigReader::init_redirection_tmp));
	this->config_field.push_back(std::make_pair("redirection", &ConfigReader::init_redirection));
	this->config_field.push_back(std::make_pair("file_system_root", &ConfigReader::init_file_system_root));
	this->config_field.push_back(std::make_pair("enable_directory_listing", &ConfigReader::init_enable_directory_listing));
	this->config_field.push_back(std::make_pair("default_filepath", &ConfigReader::init_default_filepath));
	this->config_field.push_back(std::make_pair("enable_upload", &ConfigReader::init_enable_upload));
	this->config_field.push_back(std::make_pair("CGI_pathfile", &ConfigReader::init_CGI_pathfile));
	this->config_field.push_back(std::make_pair("upload_folder", &ConfigReader::init_upload_folder));
	this->config_field.push_back(std::make_pair("client_timeout", &ConfigReader::init_client_timeout));
	this->config_field.push_back(std::make_pair("session_timeout", &ConfigReader::init_session_timeout));
	this->config_field.push_back(std::make_pair("session_cookie_name", &ConfigReader::init_session_cookie_name));
	this->config_field.push_back(std::make_pair("session_secure_only", &ConfigReader::init_session_secure_only));
	this->config_field.push_back(std::make_pair("max_sessions", &ConfigReader::init_max_sessions));
	this->config_field.push_back(std::make_pair("restrained_filepath", &ConfigReader::init_restrained_pathfile));
}

t_server_config ConfigReader::get_config()
{
	return (this->new_config);
}
