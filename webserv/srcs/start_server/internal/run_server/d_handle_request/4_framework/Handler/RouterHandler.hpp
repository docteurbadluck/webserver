#include "IRequestHandler.hpp"
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include "BaseHandler.hpp"

class RouterHandler : public IRequestHandler
{
	public :
		std::string handle( const t_parsed_request &req,
							const t_server_rules & server_rules,
							SessionHandlerUC &session_handler);
		std::vector<std::string>	get_file_content();
		int							get_fd_stream();
		pid_t						get_cgi_pid();
		int							get_close_flag();
		//int get_is_big_body();
		RouterHandler();
		~RouterHandler();
	private :
		std::map <std::string, BaseHandler*> handlers;
		BaseHandler*	last_used_handler;
		std::string	dispatch(const std::string &name, const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);
		bool	check_method_allowed(const std::string &path, const std::string &method, const std::map<std::string, std::set<std::string> > &rules);
		bool	needs_redirection(const std::string &path, const t_server_rules &server_rules) const;
		int		close_flag;
};
