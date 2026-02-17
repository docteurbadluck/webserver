#include "GetHandler.hpp"

GetHandler::GetHandler()
{
}
GetHandler::~GetHandler() {}


void GetHandler::verify_mandatory_field(const t_parsed_request &req)
{
	if (req.methode != "GET")
		throw std::runtime_error("GetHandler: Invalid method, expected GET");

	if (req.path.empty())
		throw std::runtime_error("GetHandler: Missing request target");
}

std::string GetHandler::interpret_html_body(const std::string body, SessionHandlerUC *session_handler)
{
	std::string result = body;
	size_t pos = result.find("@username");

	if (session_handler->is_valid_session(this->request.session_id) == false)
		return result;
	while (pos != std::string::npos)
	{
	    result.replace(pos, 9, session_handler->get_session(this->request.session_id).username);
	    pos = result.find("@username", pos + session_handler->get_session(this->request.session_id).username.size());
	}
	return result;
}


std::string	GetHandler::build_directory_listing(const std::string &filepath,
												const std::string &url_path)
{
	DIR *dir = opendir(filepath.c_str());
	if (!dir)
	{
		std::cerr << "ERROR: cannot open dir :" << filepath << "\n";
		return "";
	}
	std::ostringstream html;
	html << "<html><head><title>Index of " << url_path << "</title></head><body>";
	html << "<h1>Index of " << url_path << "</h1><ul>";
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		html << "<li><a href=\"" << url_path;
		if (url_path[url_path.size()-1] != '/')
			html << "/";
		html << name << "\">" << name << "</a></li>";
	}
	html << "</ul></body></html>";
	closedir(dir);
	return html.str();
}
