#include "RouterHandler.hpp"
#include "GetHandler.hpp"
#include "PostUploadHandler.hpp"
#include "PostLoginHandler.hpp"
#include "DeleteHandler.hpp"
#include "PostCloseHandler.hpp"
#include "PostTrashHandler.hpp"
#include "TrashHandler.hpp"
#include "MethodNotAllowedHandler.hpp"
#include "InternalErrorHandler.hpp"
#include "RedirectHandler.hpp"
#include "CGIHandler.hpp"


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
	handlers["MethodNotAllowedHandler"]= new MethodNotAllowedHandler();
	handlers["InternalErrorHandler"]= new InternalErrorHandler();
	handlers["RedirectHandler"]= new RedirectHandler();

	handlers["CGIHandler"] = new CGIHandler();
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
	delete handlers["MethodNotAllowedHandler"];
	delete handlers["InternalErrorHandler"];
	delete handlers["RedirectHandler"];
	delete handlers["CGIHandler"];
}

std::string RouterHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	std::map<std::string, BaseHandler*>::iterator it = handlers.find(req.methode);
	std::string result;

	if (req.path == "/return_internal_error/")
	{
		result = handlers["InternalErrorHandler"]->handle(req, server_rules, session_handler);
		close_flag = handlers["InternalErrorHandler"]->close_flag;
		return result; 
	}

	//just to show that any exeption will be catch and return a internal error (status code 500)
	if (req.path == "/cause_internal_error")
	{
		throw std::runtime_error("Forced internal error for testing");
	}

	if (needs_redirection(req.path, server_rules))
	{	
		result = handlers["RedirectHandler"]->handle(
			req, server_rules, session_handler);
		close_flag = handlers["RedirectHandler"]->close_flag;
		return result;
    }

	if (!check_method_allowed(req.path, req.methode, server_rules.restrained_filepath))
	{
		result = handlers["MethodNotAllowedHandler"]->handle(req, server_rules, session_handler);
		close_flag = handlers["MethodNotAllowedHandler"]->close_flag;
		return result;
	}

			result = handlers["CGIHandler"]->handle(req, server_rules, session_handler);
			close_flag = handlers["CGIHandler"]->close_flag;
			if (result.size())
				return result;

	
	if (it != handlers.end())
	{
			if (it->first == "POST")
			{
				if (req.path.find("/upload") != std::string::npos)
				{
					result =  handlers["POSTUpload"]->handle(req, server_rules, session_handler);
					close_flag = handlers["POSTUpload"]->close_flag;
				}
				else if (req.path.find("/login") != std::string::npos)
				{
					result =  handlers["PostLogin"]->handle(req, server_rules, session_handler);
					close_flag = handlers["PostLogin"]->close_flag;
				}
				else if (req.path.find("/close") != std::string::npos)
				{
					result =  handlers["PostClose"]->handle(req, server_rules, session_handler);
					close_flag = handlers["PostClose"]->close_flag;
				}
				else
				{
					result =  handlers["PostTrash"]->handle(req, server_rules, session_handler);
					close_flag = handlers["PostTrash"]->close_flag;
				}
			}
			else if (it->first == "GET")
			{
				result = it->second->handle(req, server_rules, session_handler);
				close_flag = handlers["GET"]->close_flag;
			}
			else
			{
				result = it->second->handle(req, server_rules, session_handler);
				close_flag = it->second->close_flag;
			}
	}
	else
	{
		result =  handlers["TrashHandler"]->handle(req, server_rules, session_handler);
		close_flag = handlers["TrashHandler"]->close_flag;
	}
	return (result);
}


std::vector<std::string>	RouterHandler::get_file_content()
{
	return (handlers["GET"]->get_file_content());
}

bool RouterHandler::check_method_allowed(const std::string &path,
                                         const std::string &method,
                                         const std::map<std::string, std::set<std::string> > &rules)
{
    for (std::map<std::string, std::set<std::string> >::const_iterator it = rules.begin();
         it != rules.end();
         ++it)
    {
        const std::string &restricted_path = it->first;
        const std::set<std::string> &allowed_methods = it->second;

        if (path.find(restricted_path) != std::string::npos)
        {
            if (allowed_methods.count(method) == 0)
                return false;
            return true;
        }
    }
    return true;
}


static bool starts_with(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size()
        && str.compare(0, prefix.size(), prefix) == 0;
}

bool RouterHandler::needs_redirection(const std::string &path,
					const t_server_rules &server_rules) const
{
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
            server_rules.redirection_permanent.begin();
         it != server_rules.redirection_permanent.end();
         ++it)
    {
        if (starts_with(path, it->first))
            return true;
    }

    for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
            server_rules.redirection_temporaire.begin();
         it != server_rules.redirection_temporaire.end();
         ++it)
    {
        if (starts_with(path, it->first))
            return true;
    }
    return false;
}


int RouterHandler::get_fd_stream()
{
    return handlers["GET"]->get_fd_stream();
}

                 
int RouterHandler::get_close_flag()
{
	return this->close_flag;
}
