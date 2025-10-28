#include "FileIO.hpp"

FileIO::FileIO(const std::string &file_path) : file_stream(file_path.c_str())
{
	if (!file_stream.is_open())
	{
		throw std::runtime_error("Cannot open file.");
	}
}

std::string FileIO::read()
{
	std::ostringstream ss;
	ss << file_stream.rdbuf();
	return ss.str();
}

void FileIO::close() {
	if (file_stream.is_open())
		file_stream.close();
}

FileIO::~FileIO()
{
	close();
}
