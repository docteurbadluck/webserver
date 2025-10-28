#include "RouterHandler.hpp"
#include "GetHandler.hpp"
#include "PostUploadHandler.hpp"
#include "PostLoginHandler.hpp"
#include "DeleteHandler.hpp"
#include "PostCloseHandler.hpp"
#include "PostTrashHandler.hpp"
#include "TrashHandler.hpp"

RouterHandler::RouterHandler()
{
	handlers["GET"] = new GetHandler();
	handlers["DELETE"] = new DeleteHandler();
	handlers["POST"] = new PostUploadHandler();
	handlers["POSTUpload"] = new PostUploadHandler();
	handlers["PostLogin"] = new PostLoginHandler();
	handlers["PostClose"] = new PostCloseHandler();
	handlers["PostTrash"] = new PostTrashHandler();
	handlers["TrashHandler"] = new TrashHandler();
}
RouterHandler::~RouterHandler()
{
	delete handlers["GET"];
	delete handlers["DELETE"];
	delete handlers["POST"];
	delete handlers["POSTUpload"];
	delete handlers["PostLogin"];
	delete handlers["PostClose"];
	delete handlers["PostTrash"];
	delete handlers["TrashHandler"];
}

std::string RouterHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	std::map<std::string, BaseHandler*>::iterator it = handlers.find(req.methode);
	std::string result;
	if (it != handlers.end())
	{
			if (it->first == "POST")
			{
				if (req.path.find("/upload") != std::string::npos)
				{
					result =  handlers["POSTUpload"]->handle(req, server_rules, session_handler);
				}
				else if (req.path.find("/login") != std::string::npos)
				{
					result =  handlers["PostLogin"]->handle(req, server_rules, session_handler);
				}
				else if (req.path.find("/close") != std::string::npos)
				{
					result =  handlers["PostClose"]->handle(req, server_rules, session_handler);
				}
				else
				{
					result =  handlers["PostTrash"]->handle(req, server_rules, session_handler);
				}
			}
			else if (it->first == "GET")
			{
				result = it->second->handle(req, server_rules, session_handler);
				get_is_big_body();
			}
			else
			{
				result = it->second->handle(req, server_rules, session_handler);
			}
	}
	else
	{
		result =  handlers["TrashHandler"]->handle(req, server_rules, session_handler);
	}
	return (result);
}


std::vector<std::string>	RouterHandler::get_file_content()
{
	return (handlers["GET"]->get_file_content());
}


int RouterHandler::get_is_big_body()
{
	return (handlers["GET"]->get_is_big_body());
}