#include "ConfigReader.hpp"

bool	ConfigReader::is_valid_file(const std::string& path)
{
	struct stat st;
	
	if (stat(path.c_str(), &st) != 0)
		return false;
	 if (!S_ISREG(st.st_mode))
		return false;
	if (access(path.c_str(), R_OK) != 0)
		return false;
	return true;
}

bool ConfigReader::is_valid_repository(const std::string& path)
{
	struct stat st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	if (!S_ISDIR(st.st_mode))
		return false;
	if (access(path.c_str(), R_OK | X_OK) != 0)
		return false;
	return true;
}
