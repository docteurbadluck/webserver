#include "BaseHandler.hpp"
#include <sys/wait.h>
#include <signal.h>
#include <cstdlib>

class CGIHandler : public BaseHandler
{
	public :
		CGIHandler(){};
		~CGIHandler(){};
		std::string handle(const t_parsed_request &req, const t_server_rules &server_rules, SessionHandlerUC &session_handler);

	private :
		void		verify_mandatory_field(const t_parsed_request &req);
		std::string build_http_response(int status_code, const std::string &body, const std::string &error_page_filepath, const std::string &filepath);
		int 		check_file_status(const std::string &file_path, const t_server_rules &server_rules);

		int			execute_cgi(const std::string &filepath, const t_parsed_request &req, const t_server_rules &server_rules);
		void		setup_env(const std::string &filepath, const t_parsed_request &req, const t_server_rules &server_rules);
		char**		build_envp();

		std::vector<std::string> env_vars;
};
