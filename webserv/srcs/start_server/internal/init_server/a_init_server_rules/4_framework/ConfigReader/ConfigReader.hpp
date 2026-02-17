#pragma once
#include "IConfigReader.hpp"
#include "../file_io/FileIO.hpp"
#include <string>
#include <cstdlib>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <sys/stat.h>
#define DEFAULT_ERROR_PAGE "./srcs/start_server/internal/init_server/a_init_server_rules/5_external_tool/default_error.html"

class ConfigReader : public IConfigReader
{
	public :

		t_server_config	get_parsed_config();
		~ConfigReader();
		ConfigReader(const std::string &config_filepath);

	private :
    #include "internal/ConfigReader_internal.hpp"
};
