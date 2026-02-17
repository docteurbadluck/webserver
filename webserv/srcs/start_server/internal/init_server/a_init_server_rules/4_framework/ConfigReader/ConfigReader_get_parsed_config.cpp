#include "ConfigReader.hpp"

t_server_config ConfigReader::get_parsed_config()
{
	this->init_configuration_struct();
	this->verify_pathfile_validity();
	this->verify_mandatory_fields(new_config);
	return (new_config);
}
