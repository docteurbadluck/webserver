#include "ConfigReader.hpp"

t_server_config ConfigReader::init_configuration_struct()
{
	std::string			line;
	std::istringstream	stream(config_content);

	while (std::getline(stream, line))
		parse_config_line(line);
	return (new_config);
}

void ConfigReader::parse_config_line(const std::string &line)
{
	size_t pos;

	for (std::vector<std::pair<std::string, ConfigHandler> >::iterator it =
			config_field.begin(); it != config_field.end(); ++it)
	{
		pos = line.find(it->first);
		if (pos == 0)
		{
			std::string value = line.substr(pos + it->first.length() + 3);
			(this->*(it->second))(value);
			break;
		}
	}
}
