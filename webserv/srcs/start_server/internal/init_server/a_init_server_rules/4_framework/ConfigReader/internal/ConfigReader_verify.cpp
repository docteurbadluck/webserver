#include "ConfigReader.hpp"

void ConfigReader::verify_pathfile_validity()
{
	verify_single_paths();
	verify_collection_paths();
}

void ConfigReader::verify_single_paths()
{
	if (this->new_config.error_page_filepath.empty() || 
		!is_valid_file(this->new_config.error_page_filepath))
		this->new_config.error_page_filepath = DEFAULT_ERROR_PAGE;

	if (!this->new_config.upload_folder.empty() &&
		!is_valid_repository(this->new_config.upload_folder))
		throw std::runtime_error("Error config filepath : upload_folder");

	if (!this->new_config.file_system_root.second.empty() &&
		!is_valid_repository(this->new_config.file_system_root.second))
		throw std::runtime_error("Error config filepath : file_system_root");
}

void ConfigReader::verify_collection_paths()
{
	for (std::vector<std::string>::iterator it = this->new_config.CGI_pathfile.begin();
		 it != this->new_config.CGI_pathfile.end(); ++it)
	{
		if (!it->empty() && !is_valid_file(*it))
			throw std::runtime_error("Error config filepath : CGI_pathfile");
	}
}

void ConfigReader::verify_mandatory_fields(const t_server_config &new_config)
{
	if (new_config.backlog <= 0)
		throw std::runtime_error("Error config : backlog is missing");
	if (new_config.ip_port_vector.empty())
		throw std::runtime_error("Error config : ip is missing");
//	if (new_config.error_page_filepath.empty())
//		throw std::runtime_error("Error config : error_page is missing");
	if (new_config.default_filepath.empty())
		throw std::runtime_error("Error config : default_filepath is missing");
	if (new_config.CGI_pathfile.empty())
		throw std::runtime_error("Error config : CGI_pathfile is missing");
	if (new_config.enable_upload == true && new_config.upload_folder.empty())
		throw std::runtime_error("Error config : upload_folder is missing");
}
