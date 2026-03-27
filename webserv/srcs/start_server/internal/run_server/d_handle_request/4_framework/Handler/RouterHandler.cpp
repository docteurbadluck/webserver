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


RouterHandler::RouterHandler() : last_used_handler(NULL)
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

std::string RouterHandler::dispatch(const std::string &name, const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	last_used_handler = handlers[name];
	std::string result = last_used_handler->handle(req, server_rules, session_handler);
	close_flag = last_used_handler->close_flag;
	return result;
}

std::string RouterHandler::handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler)
{
	if (req.path == "/return_internal_error/")
		return dispatch("InternalErrorHandler", req, server_rules, session_handler);

	if (req.path == "/cause_internal_error")
		throw std::runtime_error("Forced internal error for testing");

	if (needs_redirection(req.path, server_rules))
		return dispatch("RedirectHandler", req, server_rules, session_handler);

	if (!check_method_allowed(req.path, req.methode, server_rules.restrained_filepath))
		return dispatch("MethodNotAllowedHandler", req, server_rules, session_handler);

	std::string result = dispatch("CGIHandler", req, server_rules, session_handler);
	if (!result.empty())
		return result;

	std::map<std::string, BaseHandler*>::iterator it = handlers.find(req.methode);
	if (it != handlers.end())
	{
		if (it->first == "POST")
		{
			if (req.path.find("/upload") != std::string::npos)
				return dispatch("POSTUpload", req, server_rules, session_handler);
			else if (req.path.find("/login") != std::string::npos)
				return dispatch("PostLogin", req, server_rules, session_handler);
			else if (req.path.find("/close") != std::string::npos)
				return dispatch("PostClose", req, server_rules, session_handler);
			else
				return dispatch("PostTrash", req, server_rules, session_handler);
		}
		return dispatch(it->first, req, server_rules, session_handler);
	}
	return dispatch("TrashHandler", req, server_rules, session_handler);
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
    if (last_used_handler)
        return last_used_handler->get_fd_stream();
    return -1;
}

pid_t RouterHandler::get_cgi_pid()
{
    if (last_used_handler)
        return last_used_handler->get_cgi_pid();
    return -1;
}

                 
int RouterHandler::get_close_flag()
{
	return this->close_flag;
}
