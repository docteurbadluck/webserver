#pragma once
#include "IFile.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> 

class FileIO : public IFile
{
	public :
		FileIO(const std::string &file_path);
		std::string read();
		void close();
		~FileIO();
	private :
		std::ifstream file_stream;
};
